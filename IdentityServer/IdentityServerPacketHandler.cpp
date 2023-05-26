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

	LoginRoomPtr& room = gameState->GetRoom();
	const int64 Priority = playerState->GetSessionManager()->GetServiceTimeStamp();

	bool ret = room->PushTask(Priority, &LoginRoom::Enter, playerState);
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

	IdentityServicePtr service = std::static_pointer_cast<IdentityService>(gameState->GetService());
	if (service == nullptr)
	{
		return false;
	}

	LoginRoomPtr& room = gameState->GetRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = service->GetServiceTimeStamp();
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

	IdentityServicePtr service = std::static_pointer_cast<IdentityService>(gameState->GetService());
	if (service == nullptr)
	{
		return false;
	}

	LoginRoomPtr& room = gameState->GetRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = service->GetServiceTimeStamp();
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

	IdentityServicePtr service = std::static_pointer_cast<IdentityService>(gameState->GetService());
	if (service == nullptr)
	{
		return false;
	}

	LoginRoomPtr& room = gameState->GetRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = service->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::EmailVerified, playerState, pkt);
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

	IdentityServicePtr service = std::static_pointer_cast<IdentityService>(gameState->GetService());
	if (service == nullptr)
	{
		return false;
	}

	LoginRoomPtr& room = gameState->GetRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = service->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::LoadCharacters, playerState, pkt);
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

	IdentityServicePtr service = std::static_pointer_cast<IdentityService>(gameState->GetService());
	if (service == nullptr)
	{
		return false;
	}

	LoginRoomPtr& room = gameState->GetRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = service->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::CreateCharacter, playerState, pkt);
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

	IdentityServicePtr service = std::static_pointer_cast<IdentityService>(gameState->GetService());
	if (service == nullptr)
	{
		return false;
	}

	LoginRoomPtr& room = gameState->GetRoom();
	if (nullptr == room)
	{
		return false;
	}

	const int64 priority = service->GetServiceTimeStamp();
	bool ret = room->PushTask(priority, &LoginRoom::DeleteCharacter, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_SelectServer(PacketSessionPtr& session, Protocol::C2S_SelectServer& pkt)
{
	return true;
}

bool Handle_C2S_TravelServer(PacketSessionPtr& session, Protocol::C2S_TravelServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	return true;
}