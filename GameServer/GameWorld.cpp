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

	GameTaskPtr task = std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());
	if (nullptr == task)
	{
		return;
	}
	mEnemySpawnerManger = std::make_shared<EnemySpawnerManager>();
	mEnemySpawnerManger->SetOwner(this->GetGameObjectRef());
	task->PushTask(mEnemySpawnerManger->GetGameObjectPtr());

	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), FVector(-8000.0, 26000.0, -104.0), 2000.0f, EnemyID::Enemy_Slime, 5, -1, false, true, 1000.0f, 0.0f);
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), FVector(9600.0, 9600.0, 0.0f), 2000.0f, EnemyID::Enemy_Slime, 5, -1, false, true, 1000.0f, 0.0f);
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), FVector(-4600.0, 10000.0f, 0.0f), 0.0f, EnemyID::Enemy_Slime, 5, -1, false, true, 1000.0f, 0.0f);
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), FVector(-38000.0, 32000.0, 0.0f), 0.0f, EnemyID::Enemy_Nomal_Skeleton, 5, -1, false, true, 1000.0f, 0.0f);
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), FVector(-31000.0, 40000.0, 0.0f), 0.0f, EnemyID::Enemy_Nomal_Skeleton, 5, -1, false, true, 1000.0f, 0.0f);

	mDungeonManager = std::static_pointer_cast<DungeonManager>(SpawnActor<DungeonManager>(this->GetGameObjectRef(), Location(), FRotator(), Scale()));

	//std::shared_ptr<Portal> portal = std::static_pointer_cast<Portal>(SpawnActor<Portal>(this->GetGameObjectRef(), Location(-8300.0f, 30000.0f, -68.0f), FRotator(), Scale()));
	//portal->SetTeleportLocation(FVector(-8300.0f, 30000.0f, -68.0f));

	this->MakeWorldObstruction(EGameDataType::Obstruction);

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
		remotePlayer->OnLoadComplete();
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
			//mTokens.erase(curToken);
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
	mTaskManagerRef.lock()->PushTask(remotePlayer->GetGameObjectPtr());

	remotePlayer->SetWorld(std::static_pointer_cast<GameWorld>(GetWorldRef().lock()));
	remotePlayer->LoadRemotePlayer(token);

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
	if (false == remotePlayer->LeaveComplete())
	{
		this->GameObjectLog(L"Can't Leave remote player");
	}
}

void GameWorld::LevelTravel(GameWorldPtr inTravelWorld, int64 inLeaveRemoteID)
{

	RemoteClientPtr remoteClinet = nullptr;
	if (false == this->FindPlayer(inLeaveRemoteID, remoteClinet))
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remoteClinet);
	if (nullptr == playerState)
	{
		return;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return;
	}
	const int64& characterID = character->GetGameObjectID();

	Protocol::S2C_DisAppearCharacter disappearCharacterPacket;
	disappearCharacterPacket.set_remote_id(remotePlayer->GetGameObjectID());

	SendBufferPtr disappearSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disappearCharacterPacket);
	playerState->BroadcastPlayerMonitors(disappearSendBuffer);

	const PlayerMonitors& playerMonitors = playerState->GetPlayerMonitors();
	for (auto playerMonitor = playerMonitors.begin(); playerMonitor != playerMonitors.end();)
	{
		playerMonitor->get()->ReleasePlayerViewer(playerState);
		playerState->ReleasePlayerMonitor(*playerMonitor++);
	}

	const ActorMonitors& actorMonitors = playerState->GetActorMonitors();
	for (auto actorMonitor = actorMonitors.begin(); actorMonitor != actorMonitors.end();)
	{
		actorMonitor->get()->ReleasePlayerViewer(playerState);
		playerState->ReleaseActorMonitor(*actorMonitor++);
	}

	this->DeletePlayer(inLeaveRemoteID);
	inTravelWorld->InsertPlayer(inLeaveRemoteID, remoteClinet);
	remotePlayer->SetWorld(std::static_pointer_cast<GameWorld>(inTravelWorld));

	mWorldActors.erase(characterID);
	inTravelWorld->mWorldActors.insert(std::make_pair(characterID, character));
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

bool GameWorld::IsValidCharacter(const int64& inCharacterID, GameRemotePlayerPtr& outRemotePlayerPtr)
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

bool GameWorld::IsValidCharacter(const std::string& inCharacterName, GameRemotePlayerPtr& outRemotePlayerPtr)
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

bool GameWorld::IsValidCharacter(const int64& inCharacterID)
{
	auto findPos = mPlayerIDs.find(inCharacterID);
	if (findPos == mPlayerIDs.end())
	{
		return false;
	}

	return true;
}

bool GameWorld::IsValidPlayer(const int64& inRemoteID, GameRemotePlayerPtr& outRemotePlayerPtr)
{
	auto playerPos = mWorldPlayers.find(inRemoteID);
	if (playerPos == mWorldPlayers.end())
	{
		return false;
	}

	outRemotePlayerPtr = std::static_pointer_cast<GameRemotePlayer>(playerPos->second->GetRemotePlayer());
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

		mWorldObserver.InsertNode(actor);
	}

	//mWorldObserver.DebugKDTree();

	//for (auto& worldPlayer : mWorldPlayers)
	//{
	//	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(worldPlayer.second->GetRemotePlayer());
	//	if (nullptr == remotePlayer)
	//	{
	//		continue;
	//	}

	//	PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(remotePlayer->GetCharacter());
	//	if (nullptr == character)
	//	{
	//		continue;
	//	}

	//	const Location& actorLocation	= character->GetLocation();
	//	const int64		gameObjectID	= character->GetGameObjectID();
	//	const uint8		actorType		= character->GetActorType();

	//	mWorldObserver.InsertNode(actorLocation, gameObjectID, actorType);
	//}
}

void GameWorld::MakeWorldObstruction(const EGameDataType inGameData)
{
	mWorldObstruction.DeleteTree();

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(this->GetDatas());
	if (nullptr == datas)
	{
		return;
	}

	CSVDatas obstructionDatas;
	datas->GetData(obstructionDatas, static_cast<uint8>(inGameData));

	const size_t datasSize = obstructionDatas.size() - 1;
	for (size_t index = 1; index < datasSize; ++index)
	{
		CSVRow row = obstructionDatas.at(index);
		DoMakeWorldObstruction(row);
	}
	this->GameObjectLog(L"[GameWorld::MakeWorldObstruction] Complete a obstruction (%lld)\n", datasSize - 1);
}

void GameWorld::MakeWorldObstruction(const EGameDataType inGameData, const int32 inIndex)
{
	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(this->GetDatas());
	if (nullptr == datas)
	{
		return;
	}

	if (inIndex <= 0)
	{
		return;
	}

	CSVDatas obstructionDatas;
	datas->GetData(obstructionDatas, static_cast<uint8>(inGameData));

	CSVRow row = obstructionDatas.at(inIndex);
	DoMakeWorldObstruction(row);

}

void GameWorld::DoMakeWorldObstruction(const CSVRow& inRow)
{
	std::wstring	name = L"Obstruction";
	int32			type = std::stoi(inRow.at(1));
	FVector			location = FVector(std::stof(inRow.at(2)), std::stof(inRow.at(3)), std::stof(inRow.at(4)));
	FRotator		rotation = FRotator(std::stof(inRow.at(5)), std::stof(inRow.at(6)), std::stof(inRow.at(7)));
	FVector			scale = FVector(1.0f, 1.0f, 1.0f);
	FVector			extent = FVector(std::stof(inRow.at(8)), std::stof(inRow.at(9)), std::stof(inRow.at(10)));

	ActorPtr newActor = this->SpawnActor<Obstruction>(this->GetGameObjectRef(), location, rotation, scale);
	std::shared_ptr<Obstruction> newObstruction = std::static_pointer_cast<Obstruction>(newActor);
	if (nullptr == newObstruction)
	{
		this->GameObjectLog(L"Unable to complete a obstruction\n");
		return;
	}
	//newObstruction->SetObstructionName(name.c_str());
	newObstruction->SetObstructionType(type);
	newObstruction->SetBoxCollisionExtent(extent);

	mWorldObstruction.InsertNode(newObstruction);
}

const EnemySpawnerManagerPtr& GameWorld::GetEnemySpawnerManager()
{
	return mEnemySpawnerManger;
}

const DungeonManagerPtr& GameWorld::GetDungeonManager()
{
	return mDungeonManager;
}
