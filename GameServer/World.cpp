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
}

void World::Enter(PlayerStatePtr inPlayerState, Protocol::C2S_EnterGameServer inPacket)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (remotePlayer)
	{
		return;
	}

	if (nullptr == mGameTask)
	{
		return;
	}

	inPlayerState->SetWorld(GetWorldRef());

	remotePlayer = std::make_shared<RemotePlayer>(inPlayerState->GetPlayerStateRef());
	remotePlayer->InitTask(mGameTask);
	remotePlayer->LoadRemotePlayer(1, 0);

	std::pair<int64, PlayerStatePtr> newPlayer = std::make_pair(remotePlayer->GetGameObjectID(), inPlayerState);
	mPlayerStates.insert(newPlayer);
}

void World::Leave(PlayerStatePtr inPlayerState)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	if (nullptr == mGameTask)
	{
		return;
	}

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

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, leavePacket);
	packetSession->Send(sendBuffer);
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

bool World::DestroyActor(const int64 inGameObjectID)
{
	auto findPos = mWorldActors.find(inGameObjectID);
	if (findPos == mWorldActors.end())
	{
		return false;
	}

	size_t result = mWorldActors.erase(inGameObjectID);
	return (result != 0) ? true : false;
}