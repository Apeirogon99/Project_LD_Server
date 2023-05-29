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

	WorldPtr world = gameState->GetWorld();
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

	session->Disconnect(L"LeaveGameServer");
	return true;
}

bool Handle_C2S_MovementCharacter(PacketSessionPtr& session, Protocol::C2S_MovementCharacter& pkt)
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

	WorldPtr world = gameState->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &World::MoveDestination, playerState, pkt);
	return true;
}

bool Handle_C2S_LoadInventory(PacketSessionPtr& session, Protocol::C2S_LoadInventory& pkt)
{
	return false;
}

bool Handle_C2S_InsertInventory(PacketSessionPtr& session, Protocol::C2S_InsertInventory& pkt)
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

	WorldPtr world = gameState->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	world->PushTask(timestmap, &World::InsertItemToInventory, playerState, pkt);

	return true;
}

bool Handle_C2S_UpdateInventory(PacketSessionPtr& session, Protocol::C2S_UpdateInventory& pkt)
{
	return true;
}

bool Handle_C2S_DeleteInventory(PacketSessionPtr& session, Protocol::C2S_DeleteInventory& pkt)
{
	return true;
}
