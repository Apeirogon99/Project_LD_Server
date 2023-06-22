#include "pch.h"
#include "World.h"

World::World(const WCHAR* inName, GameTaskPtr inGameTask) : GameObject(inName), mGameTask(inGameTask)
{
	
}

World::~World()
{
	
}

void World::Initialization()
{
	Protocol::SVector loc;
	loc.set_x(0);
	loc.set_y(0);
	loc.set_z(500);

	Protocol::SRotator rot;
	rot.set_pitch(0);
	rot.set_yaw(0);
	rot.set_roll(0);

	AItemPtr item = std::static_pointer_cast<AItem>(CreateActor<AItem>(loc, rot));
	item->SetItemCode(31);
}

void World::Destroy()
{
	if (mGameTask)
	{
		mGameTask.reset();
	}
}

bool World::IsValid()
{
	return true;
}

void World::Tick()
{
	VisibleAreaSync();
	//CheackToken();
}

void World::ServerTravel(PlayerStatePtr inPlayerState, Protocol::C2S_TravelServer inPacket)
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

void World::Enter(PlayerStatePtr inPlayerState, Protocol::C2S_EnterGameServer inPacket)
{

	GameStatePtr gameState = std::static_pointer_cast<GameState>(inPlayerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return;
	}

	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (remotePlayer)
	{
		return;
	}

	if (nullptr == mGameTask)
	{
		return;
	}

	Token token;
	bool isToken = false;
	if (inPacket.token().compare("LOCAL_TEST") == 0)
	{
		isToken = true;
		token.SetCharacterID(0);
		token.SetGlobalID(0);
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
	inPlayerState->SetWorld(GetWorldRef());

	remotePlayer = std::make_shared<RemotePlayer>(inPlayerState->GetPlayerStateRef());
	remotePlayer->InitTask(mGameTask);
	remotePlayer->LoadRemotePlayer(token);

}

void World::Leave(PlayerStatePtr inPlayerState)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);

	if (nullptr == mGameTask)
	{
		return;
	}

	Protocol::S2C_DisAppearCharacter disappearCharacterPacket;
	disappearCharacterPacket.set_remote_id(remotePlayer->GetGameObjectID());

	SendBufferPtr disappearSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disappearCharacterPacket);
	inPlayerState->BroadcastMonitors(disappearSendBuffer);

	Monitors& monitors = inPlayerState->GetMonitors();
	for (auto monitor = monitors.begin(); monitor != monitors.end(); ++monitor)
	{
		monitor->get()->GetViewers().erase(inPlayerState);
	}
	
	const int64 gameObjectID = remotePlayer->GetGameObjectID();
	mPlayerStates.erase(gameObjectID);
	remotePlayer->DestroyTask(mGameTask);

	Protocol::S2C_LeaveGameServer leavePacket;
	leavePacket.set_error(true);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, leavePacket);
	packetSession->Send(sendBuffer);
}

void World::VisibleAreaInit(PlayerStatePtr inPlayerState)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	remotePlayer->GetViewers().insert(inPlayerState);
	inPlayerState->GetMonitors().insert(remotePlayer);

	auto oldPlayer = mPlayerStates.begin();
	for (oldPlayer; oldPlayer != mPlayerStates.end(); oldPlayer++)
	{

		remotePlayer->GetCharacter()->CloseToPlayer(oldPlayer->second);

		oldPlayer->second->GetRemotePlayer()->GetCharacter()->CloseToPlayer(inPlayerState);
		

		auto viewActor = mWorldActors.begin();
		for (viewActor; viewActor != mWorldActors.end(); viewActor++)
		{
			viewActor->second->CloseToPlayer(inPlayerState);
		}

	}

	std::pair<int64, PlayerStatePtr> newPlayer = std::make_pair(remotePlayer->GetGameObjectID(), inPlayerState);
	mPlayerStates.insert(newPlayer);
}

void World::VisibleAreaSync()
{
	auto player = mPlayerStates.begin();
	for (player; player != mPlayerStates.end(); player++)
	{
		auto viewPlayer = mPlayerStates.begin();
		for (viewPlayer; viewPlayer != mPlayerStates.end(); viewPlayer++)
		{
			player->second->GetRemotePlayer()->GetCharacter()->CloseToPlayer(viewPlayer->second);
		}

		auto viewActor = mWorldActors.begin();
		for (viewActor; viewActor != mWorldActors.end(); viewActor++)
		{
			viewActor->second->CloseToPlayer(player->second);
		}
	}
}

void World::CheackToken()
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

bool World::DestroyActor(const int64 inGameObjectID)
{
	auto findPos = mWorldActors.find(inGameObjectID);
	if (findPos == mWorldActors.end())
	{
		return false;
	}

	for (auto& playerState : mPlayerStates)
	{
		findPos->second->DisAppearActor(playerState.second);
	}

	mGameTask->ReleaseTask(findPos->second->GetGameObjectPtr());

	if (0 == mWorldActors.erase(inGameObjectID))
	{
		return false;
	}

	return true;
}

bool World::IsValidActor(const int64 inGameObjectID)
{
	auto findPos = mWorldActors.find(inGameObjectID);
	if (findPos == mWorldActors.end())
	{
		return false;
	}

	return true;
}
