#include "pch.h"
#include "IdentityServerPacketHandler.h"

bool Handle_C2S_EnterIdentityServer(PacketSessionPtr& session, Protocol::C2S_EnterIdentityServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	const int64 Priority = gameState->GetServiceTimeStamp();
	bool ret = world->PushTask(Priority, &World::Enter, playerState);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_LeaveIdentityServer(PacketSessionPtr& session, Protocol::C2S_LeaveIdentityServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	playerState->Disconnect(L"Leave identity server packet");

	return true;
}

bool Handle_C2S_Singin(PacketSessionPtr& session, Protocol::C2S_Singin& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	LoginRoomPtr room = world->GetLoginRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::Signin, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_Singup(PacketSessionPtr& session, Protocol::C2S_Singup& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	LoginRoomPtr room = world->GetLoginRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::Signup, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_EmailVerified(PacketSessionPtr& session, Protocol::C2S_EmailVerified& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	LoginRoomPtr room = world->GetLoginRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::EmailVerified, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_LoadServer(PacketSessionPtr& session, Protocol::C2S_LoadServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	LoginRoomPtr room = world->GetLoginRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::LoadServerRequest, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_SelectServer(PacketSessionPtr& session, Protocol::C2S_SelectServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	LoginRoomPtr room = world->GetLoginRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::SelectServer, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_StartGame(PacketSessionPtr& session, Protocol::C2S_StartGame& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	SelectRoomPtr room = world->GetSelectRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &SelectRoom::StartCharacterRequest, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}


bool Handle_C2S_LoadCharacters(PacketSessionPtr& session, Protocol::C2S_LoadCharacters& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	SelectRoomPtr room = world->GetSelectRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &SelectRoom::LoadCharacters, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_CreateCharacter(PacketSessionPtr& session, Protocol::C2S_CreateCharacter& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	CustomRoomPtr room = world->GetCustomRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &CustomRoom::CreateCharacter, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_DeleteCharacter(PacketSessionPtr& session, Protocol::C2S_DeleteCharacter& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	SelectRoomPtr room = world->GetSelectRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = gameState->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &SelectRoom::DeleteCharacter, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}