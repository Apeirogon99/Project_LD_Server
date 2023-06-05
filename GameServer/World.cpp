#include "pch.h"
#include "World.h"

World::World(const WCHAR* inName) : GameObject(inName), mPlayersCount(0), mGameObjectID(1)
{
	
}

World::~World()
{
	
}

void World::Initialization()
{
	mPlayersCount = 0;
	mGameObjectID = 1;

	CreateTempItem();
}

void World::Destroy()
{
	mPlayers.clear();
}

bool World::IsValid()
{
	return true;
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

	Protocol::S2C_AppearItem appearItemPacket;
	for (auto actor : mActors)
	{
		AItemPtr item = std::static_pointer_cast<AItem>(actor.second);
		if (item)
		{
			Protocol::SItem* addItem = appearItemPacket.add_item();
			addItem->set_object_id(item->GetGameObjectID());
			addItem->set_item_code(item->mItemCode);

			Protocol::SVector* worldPosition = addItem->mutable_world_position();
			*worldPosition = item->GetLocation();

			Protocol::SVector2D* invenPositon = addItem->mutable_inven_position();
			invenPositon->set_x(item->mInventoryPositionX);
			invenPositon->set_y(item->mInventoryPositionY);

			addItem->set_rotation(item->mRotation);
		}
	}
	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, appearItemPacket);
	packetSession->Send(appearItemSendBuffer);
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

	if (false == IsValidPlayer(remotePlayer))
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

	if (false == IsValidPlayer(remotePlayer))
	{
		return;
	}

	Protocol::S2C_LoadInventory loadInventoryPacket;
	remotePlayer->GetInventory()->LoadItem(loadInventoryPacket);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, loadInventoryPacket);
	packetSession->Send(sendBuffer);
}

void World::UpdateItemToInventory(PlayerStatePtr inPlayerState, Protocol::C2S_UpdateInventory inPacket)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	if (false == IsValidPlayer(remotePlayer))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_UpdateInventory_Requset(packetSession, inPacket);
}

void World::DeleteItemToInventory(PlayerStatePtr inPlayerState, Protocol::C2S_DeleteInventory inPacket)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	if (false == IsValidPlayer(remotePlayer))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_DeleteInventory_Requset(packetSession, inPacket);
}

bool World::InsertActor(const int64 inGameObjectID, ActorPtr inActor)
{
	std::pair<int64, ActorPtr> newObject = std::make_pair(inGameObjectID, inActor);
	auto result = mActors.insert(newObject);
	return result.second;
}

bool World::DestroyActor(const int64 inGameObjectID)
{
	auto findPos = mActors.find(inGameObjectID);
	if (findPos == mActors.end())
	{
		return false;
	}

	size_t result = mActors.erase(inGameObjectID);
	return (result != 0) ? true : false;
}

WorldRef World::GetWorldRef()
{
	return std::static_pointer_cast<World>(shared_from_this());
}

const int64 World::GetNextGameObjectID()
{
	return mGameObjectID++;
}

bool World::IsValidPlayer(RemotePlayerPtr inRemotePlayer)
{
	auto remotePlayer = mPlayers.find(inRemotePlayer->GetRemoteID());
	return (remotePlayer != mPlayers.end()) ? true : false;
}

bool World::IsValidGameObject(ActorPtr inActor)
{
	auto gameObject = mActors.find(inActor->GetGameObjectID());
	return (gameObject != mActors.end()) ? true : false;
	return false;
}
