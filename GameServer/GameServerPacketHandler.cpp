#include "pch.h"
#include "GameServerPacketHandler.h"

using namespace std;

bool Handle_C2S_EnterGameServer(PacketSessionPtr& session, Protocol::C2S_EnterGameServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if(nullptr == gameState)
	{
		return false;
	}

	GameTaskPtr task = gameState->GetGameTask();
	if (nullptr == task)
	{
		return false;
	}

	GameWorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}
	
	const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &GameWorld::Enter, playerState, pkt);
	return true;
}

bool Handle_C2S_LeaveGameServer(PacketSessionPtr& session, Protocol::C2S_LeaveGameServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return false;
	}

	GameTaskPtr task = gameState->GetGameTask();
	if (nullptr == task)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	session->Disconnect(L"Handle_C2S_LeaveGameServer");
	
	/*const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &World::Leave, playerState);*/
	return true;
}

bool Handle_C2S_Tick(PacketSessionPtr& session, Protocol::C2S_Tick& pkt)
{
	return true;
}

bool Handle_C2S_MovementCharacter(PacketSessionPtr& session, Protocol::C2S_MovementCharacter& pkt)
{

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return false;
	}

	remotePlayer->GetCharacter()->PushTask(pkt.timestamp(), &PlayerCharacter::MovementCharacter, pkt);
	return true;
}

bool Handle_C2S_PlayAnimation(PacketSessionPtr& session, Protocol::C2S_PlayAnimation& pkt)
{
	return false;
}

bool Handle_C2S_PlayerAutoAttack(PacketSessionPtr& session, Protocol::C2S_PlayerAutoAttack& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return false;
	}


	character->PushTask(pkt.timestamp(), &PlayerCharacter::AutoAttack, pkt.object_id());
	return true;
}

bool Handle_C2S_LoadInventory(PacketSessionPtr& session, Protocol::C2S_LoadInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	remotePlayer->GetInventory()->PushTask(pkt.timestamp(), &Inventory::LoadItemToInventory, pkt);
	return true;
}

bool Handle_C2S_InsertInventory(PacketSessionPtr& session, Protocol::C2S_InsertInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	AItemPtr newItem = std::make_shared<AItem>();
	newItem->Init(pkt.item());

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::InsertItemToInventory, newItem);

	return true;
}

bool Handle_C2S_UpdateInventory(PacketSessionPtr& session, Protocol::C2S_UpdateInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::UpdateItemToInventory, pkt);

	return true;
}

bool Handle_C2S_DeleteInventory(PacketSessionPtr& session, Protocol::C2S_DeleteInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::DeleteItemToInventory, pkt);
	return true;
}

bool Handle_C2S_ReplaceEqipment(PacketSessionPtr& session, Protocol::C2S_ReplaceEqipment& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::ReplcaeItemToEqipment, pkt);
	return true;
}