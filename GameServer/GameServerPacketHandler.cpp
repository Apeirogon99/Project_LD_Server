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

	WorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}
	
	const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &World::Enter, playerState);
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
	
	const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &World::Leave, playerState);
	return true;
}

bool Handle_C2S_MovementCharacter(PacketSessionPtr& session, Protocol::C2S_MovementCharacter& pkt)
{

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return false;
	}

	remotePlayer->GetCharacter()->PushTask(pkt.timestamp(), &Character::MoveDestination, pkt);
	return true;
}

bool Handle_C2S_LoadInventory(PacketSessionPtr& session, Protocol::C2S_LoadInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::LoadItemToInventory, pkt);

	return true;
}

bool Handle_C2S_InsertInventory(PacketSessionPtr& session, Protocol::C2S_InsertInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::InsertItemToInventory, pkt);

	return true;
}

bool Handle_C2S_UpdateInventory(PacketSessionPtr& session, Protocol::C2S_UpdateInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
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

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::DeleteItemToInventory, pkt);
	return true;
}

bool Handle_C2S_InsertEqipment(PacketSessionPtr& session, Protocol::C2S_InsertEqipment& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::InsertItemToEqipment, pkt);
	return true;
}

bool Handle_C2S_DeleteEqipment(PacketSessionPtr& session, Protocol::C2S_DeleteEqipment& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::DeleteItemToEqipment, pkt);
	return true;
}
