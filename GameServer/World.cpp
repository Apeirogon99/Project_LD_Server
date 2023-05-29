#include "pch.h"
#include "World.h"

World::World(const SessionManagerRef& inSessionManager, const WCHAR* inName) : GameObject(inSessionManager, inName), mPlayersCount(0), mGameObjectID(0)
{
}

World::~World()
{
}

void World::Initialization()
{
	mPlayersCount = 0;
	mGameObjectID = 0;
}

void World::Destroy()
{
	mPlayers.clear();
}

void World::Tick()
{
}

void World::Enter(PlayerStatePtr inPlayerState)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (remotePlayer)
	{
		return;
	}

	static int64 remoteID = 1;

	remotePlayer = std::make_shared<RemotePlayer>(inPlayerState->GetPlayerStateRef(), remoteID);
	remotePlayer->Init();

	auto result = mPlayers.insert(std::make_pair(remoteID, inPlayerState));
	if (false == result.second)
	{
		inPlayerState->Disconnect(L"Do not create remote player");
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_LoadInventory_Requset(packetSession, 0);

	Protocol::S2C_EnterGameServer enterPacket;
	enterPacket.set_remote_id(remoteID);
	enterPacket.set_error(result.second);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, enterPacket);
	packetSession->Send(sendBuffer);

	mPlayersCount++;
	remoteID++;

	for (auto& player : mPlayers)
	{
		if (player.second != inPlayerState)
		{
			AppearCharacter(player.second, inPlayerState);
			AppearCharacter(inPlayerState, player.second);
		}
	}

	CreateItem(inPlayerState);

}

void World::Leave(PlayerStatePtr inPlayerState)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	const int64 remoteID = remotePlayer->GetRemoteID();
	size_t result = mPlayers.erase(remoteID);
	int32 error = (result == 0) ? false : true;

	Protocol::S2C_LeaveGameServer leavePacket;
	leavePacket.set_error(error);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, leavePacket);
	packetSession->Send(sendBuffer);


	for (auto& player : mPlayers)
	{
		if (player.second != inPlayerState)
		{
			DisAppearCharacter(player.second, inPlayerState);
			DisAppearCharacter(inPlayerState, player.second);
		}
	}

	mPlayersCount--;
}

void World::Broadcast(SendBufferPtr sendBuffer)
{
	for (auto& player : mPlayers)
	{
		if (player.second)
		{
			player.second->Send(sendBuffer);
		}
	}
}

void World::AppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState)
{
	RemotePlayerPtr& appearRemotePlayer = inAppearPlayerState->GetRemotePlayer();
	if (nullptr == appearRemotePlayer)
	{
		return;
	}

	Protocol::S2C_AppearCharacter appearPacket;
	Protocol::SCharacterData* characterData = appearPacket.mutable_character_data();
	//*characterData = appearRemotePlayer->mCharacterData;
	appearPacket.set_remote_id(appearRemotePlayer->GetRemoteID());

	if (nullptr == inTargetPlayerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inTargetPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, appearPacket);
	packetSession->Send(sendBuffer);

}

void World::DisAppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState)
{
	RemotePlayerPtr& disappearRemotePlayer = inAppearPlayerState->GetRemotePlayer();
	if (nullptr == disappearRemotePlayer)
	{
		return;
	}

	Protocol::S2C_DisAppearCharacter disappearPacket;
	disappearPacket.set_remote_id(disappearRemotePlayer->GetRemoteID());

	if (nullptr == inTargetPlayerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inTargetPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disappearPacket);
	packetSession->Send(sendBuffer);

}

void World::MoveDestination(PlayerStatePtr inPlayerState, Protocol::C2S_MovementCharacter inPakcet)
{
	RemotePlayerPtr remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	Protocol::STransform	transform = inPakcet.transform();
	const int64				remoteID = remotePlayer->GetRemoteID();
	const int64				timestamp = inPakcet.timestamp();

	Protocol::S2C_MovementCharacter newMovementPacket;
	Protocol::STransform* newTransform = newMovementPacket.mutable_transform();
	*newTransform = transform;
	newMovementPacket.set_remote_id(remoteID);
	newMovementPacket.set_timestamp(timestamp);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, newMovementPacket);
	Broadcast(sendBuffer);
}

void World::InsertItemToInventory(PlayerStatePtr inPlayerState, Protocol::C2S_InsertInventory inPacket)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	if (false == IsValid(remotePlayer))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_InsertInventory_Requset(packetSession, inPacket);

}

void World::LoadItemToInventory(PlayerStatePtr inPlayerState, Protocol::C2S_LoadInventory inPacket)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	if (false == IsValid(remotePlayer))
	{
		return;
	}

	Protocol::S2C_LoadInventory loadInventoryPacket;
	remotePlayer->GetInventory()->LoadItem(loadInventoryPacket);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, loadInventoryPacket);
	packetSession->Send(sendBuffer);
}

void World::CreateItem(PlayerStatePtr inPlayerState)
{
	for (int32 i = 1; i <= 3; ++i)
	{
		Protocol::S2C_CreateItem createItem;
		Protocol::SItem* item = createItem.mutable_item();
		item->set_object_id(i);
		item->set_item_code(i);
		Protocol::SVector* position = item->mutable_world_position();
		position->set_x(i * 100);
		position->set_y(50);
		position->set_z(50);

		PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, createItem);
		inPlayerState->Send(sendBuffer);
	}

}

const int64 World::GetNextGameObjectID()
{
	return mGameObjectID++;
}

bool World::IsValid(RemotePlayerPtr inRemotePlayer)
{
	auto remotePlayer = mPlayers.find(inRemotePlayer->GetRemoteID());
	return (remotePlayer != mPlayers.end()) ? true : false;
}
