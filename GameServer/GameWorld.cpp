#include "pch.h"
#include "GameWorld.h"

GameWorld::GameWorld(const WCHAR* inName) : World(inName)
{
	
}

GameWorld::~GameWorld()
{
	
}

void GameWorld::OnInitialization()
{
	this->SetTick(true, SYSTEM_TICK);
	mSpawnerManager = std::static_pointer_cast<EnemySpawnerManager>(SpawnActor<EnemySpawnerManager>(this->GetGameObjectRef(), Location(), FRotator(), Scale()));
}

void GameWorld::OnDestroy()
{
	TaskManagerPtr taskManager = this->GetTaskManagerRef().lock();
	if (taskManager)
	{
		taskManager.reset();
	}

	DestroyAllActor();
}

bool GameWorld::IsValid()
{
	return true;
}

void GameWorld::OnTick(const int64 inDeltaTime)
{
	const int64 nextWorldTime = this->GetNextWorldTime();

	VisibleAreaSync(inDeltaTime);

	//CheackToken();

	this->PushTask(nextWorldTime, &GameWorld::RefreshWorldObserver);
}

void GameWorld::ServerTravel(PlayerStatePtr inPlayerState, Protocol::C2S_TravelServer inPacket)
{

	GameStatePtr gameState = std::static_pointer_cast<GameState>(inPlayerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return;
	}

	bool isToken = true;
	for (Token& token : mTokens)
	{
		if (token.CompareToken(inPacket.token()))
		{
			isToken = false;
		}
	}

	Protocol::S2C_TravelServer travelServer;
	if (true == isToken)
	{
		Token newToken;
		newToken.SetToken(inPacket.token());
		newToken.SetGlobalID(inPacket.global_id());
		newToken.SetCharacterID(inPacket.character_id());
		newToken.SetLastTick(gameState->GetServiceTimeStamp());
		mTokens.push_back(newToken);


		travelServer.set_token(newToken.GetToken());
		travelServer.set_server_id(gameState->GetServerID());
		travelServer.set_travel_level(gameState->GetServerLevel());
		travelServer.set_ip(gameState->GetServerIP());
		travelServer.set_port(gameState->GetServerPort());
		travelServer.set_error(ErrorToInt(EGameErrorType::SUCCESS));
	}
	else
	{
		travelServer.set_error(ErrorToInt(EGameErrorType::INVALID_TOKEN));
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(packetSession, travelServer);
	packetSession->Send(sendBuffer);	
}

void GameWorld::Enter(PlayerStatePtr inPlayerState, Protocol::C2S_EnterGameServer inPacket)
{

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (remotePlayer)
	{
		return;
	}

	Token token;
	bool isToken = false;
	if (inPacket.token().compare("LOCAL_TEST") == 0)
	{
		isToken = true;
		if (mPlayerIDs.find(1) == mPlayerIDs.end())
		{
			token.SetCharacterID(1);
			token.SetGlobalID(1);
		}
		else
		{
			token.SetCharacterID(2);
			token.SetGlobalID(2);
		}
	}

	for (auto curToken = mTokens.begin(); curToken != mTokens.end(); curToken++)
	{
		if (curToken->CompareToken(inPacket.token()))
		{
			token = *curToken;
			mTokens.erase(curToken);
			isToken = true;
			break;
		}
	}

	if (false == isToken)
	{
		Protocol::S2C_EnterGameServer enterPacket;
		enterPacket.set_error(ErrorToInt(EGameErrorType::INVALID_TOKEN));

		PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, enterPacket);
		packetSession->Send(sendBuffer);
		return;
	}

	remotePlayer = std::make_shared<GameRemotePlayer>();
	remotePlayer->SetRemoteClient(inPlayerState);
	inPlayerState->SetRemotePlayer(remotePlayer);
	mTaskManagerRef.lock()->CreateGameObject(remotePlayer->GetGameObjectPtr());

	remotePlayer->LoadRemotePlayer(token, std::static_pointer_cast<GameWorld>(GetWorldRef().lock()));

}

void GameWorld::Leave(PlayerStatePtr inPlayerState)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);

	GameTaskPtr gameTaskManager = std::static_pointer_cast<GameTask>(GetTaskManagerRef().lock());
	if (nullptr == gameTaskManager)
	{
		return;
	}

	Protocol::S2C_DisAppearCharacter disappearCharacterPacket;
	disappearCharacterPacket.set_remote_id(remotePlayer->GetGameObjectID());

	SendBufferPtr disappearSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disappearCharacterPacket);
	inPlayerState->BroadcastPlayerMonitors(disappearSendBuffer);

	const PlayerMonitors& playerMonitors = inPlayerState->GetPlayerMonitors();
	for (auto playerMonitor = playerMonitors.begin(); playerMonitor != playerMonitors.end();)
	{
		playerMonitor->get()->ReleasePlayerViewer(inPlayerState);
		inPlayerState->ReleasePlayerMonitor(*playerMonitor++);
	}

	const ActorMonitors& actorMonitors = inPlayerState->GetActorMonitors();
	for (auto actorMonitor = actorMonitors.begin(); actorMonitor != actorMonitors.end();)
	{
		actorMonitor->get()->ReleasePlayerViewer(inPlayerState);
		inPlayerState->ReleaseActorMonitor(*actorMonitor++);
	}

	const int64 gameObjectID = remotePlayer->GetGameObjectID();
	this->ReleaseCharacterIDandRemoteID(remotePlayer->GetToken().GetCharacterID());
	mWorldPlayers.erase(gameObjectID);

	remotePlayer->LeaveRemotePlayer();
	if (true == remotePlayer->LeaveComplete())
	{
		mTaskManagerRef.lock()->DestroyGameObject(remotePlayer->GetGameObjectPtr());

		Protocol::S2C_LeaveGameServer leavePacket;
		leavePacket.set_error(true);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, leavePacket);
		packetSession->Send(sendBuffer);
	}

}

void GameWorld::WorldChat(PlayerStatePtr inPlayerState, const int64 inWorldTime, std::string inMessage)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerCharacterPtr playerCharacter = remotePlayer->GetCharacter();
	if (nullptr == playerCharacter)
	{
		return;
	}

	Protocol::S2C_Chat chatPacket;
	chatPacket.set_remote_id(remotePlayer->GetGameObjectID());
	chatPacket.set_name(playerCharacter->GetCharacterData().name());
	chatPacket.set_message(inMessage);
	chatPacket.set_chat_type(Protocol::Chat_World);
	chatPacket.set_timestamp(inWorldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, chatPacket);
	this->SendWorldPlayers(sendBuffer);
}

void GameWorld::VisibleAreaInit(PlayerStatePtr inPlayerState)
{
	GameRemotePlayerPtr newRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == newRemotePlayer)
	{
		return;
	}

	PlayerCharacterPtr newCharacter = newRemotePlayer->GetCharacter();
	if (nullptr == newCharacter)
	{
		return;
	}

	newRemotePlayer->InsertPlayerViewer(inPlayerState);
	inPlayerState->InsertPlayerMonitor(newRemotePlayer);

	auto oldPlayer = mWorldPlayers.begin();
	for (oldPlayer; oldPlayer != mWorldPlayers.end(); oldPlayer++)
	{

		GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer->second->GetRemotePlayer());
		PlayerCharacterPtr	oldCharacter	= oldRemotePlayer->GetCharacter();
		
		newCharacter->CloseToActor(oldCharacter, MAX_POSSIBLE_AREA);
		oldCharacter->CloseToActor(newCharacter, MAX_POSSIBLE_AREA);

	}

	auto viewActor = mWorldActors.begin();
	for (viewActor; viewActor != mWorldActors.end(); viewActor++)
	{
		if (newCharacter != viewActor->second)
		{
			viewActor->second->CloseToActor(newCharacter, MAX_POSSIBLE_AREA);
		}
	}

	std::pair<int64, RemoteClientPtr> newPlayer = std::make_pair(newRemotePlayer->GetGameObjectID(), inPlayerState);
	mWorldPlayers.insert(newPlayer);
}

void GameWorld::VisibleAreaSync(const int64 inDeltaTime)
{
	static int64 visibleSyncTime = 0;
	visibleSyncTime += inDeltaTime;
	if (visibleSyncTime < SYSTEM_TICK)
	{
		return;
	}
	visibleSyncTime = 0;

	auto player = mWorldPlayers.begin();
	for (player; player != mWorldPlayers.end(); player++)
	{

		GameRemotePlayerPtr playerRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(player->second->GetRemotePlayer());

		auto viewPlayer = mWorldPlayers.begin();
		for (viewPlayer; viewPlayer != mWorldPlayers.end(); viewPlayer++)
		{
			GameRemotePlayerPtr viewRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(viewPlayer->second->GetRemotePlayer());
			playerRemotePlayer->GetCharacter()->CloseToActor(viewRemotePlayer->GetCharacter(), MAX_POSSIBLE_AREA);
		}

		auto viewActor = mWorldActors.begin();
		for (viewActor; viewActor != mWorldActors.end(); viewActor++)
		{
			if (playerRemotePlayer->GetCharacter() != viewActor->second)
			{
				viewActor->second->CloseToActor(playerRemotePlayer->GetCharacter(), MAX_POSSIBLE_AREA);
			}
		}
	}
}

void GameWorld::CheackToken()
{

	static int32 MAX_LAST_TICK = 60000;

	auto token = mTokens.begin();
	for (token; token != mTokens.end();)
	{
		if (token->GetLastTick() > MAX_LAST_TICK)
		{
			mTokens.erase(token);
		}
		++token;
	}
}

void GameWorld::PushCharacterIDandRemoteID(const int64& inCharacterID, const std::string& inCharacterName, const int64& inRemoteID)
{
	WorldPlayerInfo worldPlayerInfo;
	worldPlayerInfo.SetRemoteID(inRemoteID);
	worldPlayerInfo.SetCharacterName(inCharacterName);

	std::pair<int64, WorldPlayerInfo> ids = std::make_pair(inCharacterID, worldPlayerInfo);
	this->mPlayerIDs.insert(ids);
}

void GameWorld::ReleaseCharacterIDandRemoteID(const int64& inCharacterID)
{
	mPlayerIDs.erase(inCharacterID);
}

bool GameWorld::IsValidPlayer(const int64& inCharacterID, GameRemotePlayerPtr& outRemotePlayerPtr)
{
	auto findPos = mPlayerIDs.find(inCharacterID);
	if (findPos == mPlayerIDs.end())
	{
		return false;
	}

	auto playerPos = mWorldPlayers.find(findPos->second.GetRemoteID());
	if (playerPos == mWorldPlayers.end())
	{
		return false;
	}

	outRemotePlayerPtr = std::static_pointer_cast<GameRemotePlayer>(playerPos->second->GetRemotePlayer());
	return true;
}

bool GameWorld::IsValidPlayer(const std::string& inCharacterName, GameRemotePlayerPtr& outRemotePlayerPtr)
{
	auto findPos = mPlayerIDs.begin();
	for (findPos; findPos != mPlayerIDs.end(); ++findPos)
	{
		if (0 == inCharacterName.compare(findPos->second.GetCharacterName()))
		{
			break;
		}
	}

	if (findPos == mPlayerIDs.end())
	{
		return false;
	}

	auto playerPos = mWorldPlayers.find(findPos->second.GetRemoteID());
	if (playerPos == mWorldPlayers.end())
	{
		return false;
	}

	outRemotePlayerPtr = std::static_pointer_cast<GameRemotePlayer>(playerPos->second->GetRemotePlayer());
	return true;
}

bool GameWorld::IsValidPlayer(const int64& inCharacterID)
{
	auto findPos = mPlayerIDs.find(inCharacterID);
	if (findPos == mPlayerIDs.end())
	{
		return false;
	}

	return true;
}

void GameWorld::RefreshWorldObserver()
{
	mWorldObserver.DeleteTree();

	for (auto& worldActor : mWorldActors)
	{
		ActorPtr actor = worldActor.second;
		if (!actor && !actor->IsValid())
		{
			continue;
		}

		const Location& actorLocation	= actor->GetLocation();
		const int64		gameObjectID	= actor->GetGameObjectID();
		const uint8		actorType		= actor->GetActorType();

		mWorldObserver.InsertNode(actorLocation, gameObjectID, actorType);
	}

	for (auto& worldPlayer : mWorldPlayers)
	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(worldPlayer.second->GetRemotePlayer());
		if (nullptr == remotePlayer)
		{
			continue;
		}

		PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(remotePlayer->GetCharacter());
		if (nullptr == character)
		{
			continue;
		}

		const Location& actorLocation	= character->GetLocation();
		const int64		gameObjectID	= character->GetGameObjectID();
		const uint8		actorType		= character->GetActorType();

		mWorldObserver.InsertNode(actorLocation, gameObjectID, actorType);
	}
}

const EnemySpawnerManagerPtr& GameWorld::GetEnemySpawnerManager()
{
	return mSpawnerManager;
}