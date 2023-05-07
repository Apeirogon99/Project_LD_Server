#include "pch.h"
#include "IdentityServerPacketHandler.h"

bool Handle_INVALID(PacketSessionPtr& session, BYTE* buffer, int32 len)
{
	return false;
}

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

	static int64 newRemoteID = 1;
	RemotePlayerPtr& remotePlayer = playerState.get()->mRemotePlayer;
	remotePlayer = std::make_shared<RemotePlayer>();
	if (remotePlayer)
	{
		remotePlayer->mRemoteID = newRemoteID++;
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

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	LoginRoomPtr& room = gameState->GetRoom();
	const int64		Priority = gameState->GetServiceTimeStamp();

	bool ret = room->PushTask(Priority, &LoginRoom::Leave, playerState);
	if (false == ret)
	{
		return false;
	}

	gameState->ReleaseSession(playerState);
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

	Handle_Singin_Requset(session, pkt);

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

	Handle_Singup_Requset(session, pkt);

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

	Handle_EmailVerified_Requset(session, pkt);

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

	Handle_LoadCharacters_Requset(session, pkt);

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

	Handle_CreateCharacter_Requset(session, pkt);

	return true;
}

bool Handle_C2S_UpdateAppearance(PacketSessionPtr& session, Protocol::C2S_UpdateAppearance& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	Handle_UpdateAppearance_Requset(session, pkt);

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

	Handle_DeleteCharacter_Requset(session, pkt);

	return true;
}

bool Handle_C2S_UpdateNickName(PacketSessionPtr& session, Protocol::C2S_UpdateNickName& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	Handle_UpdateNickName_Requset(session, pkt);

	return true;
}

bool Handle_C2S_TravelLevel(PacketSessionPtr& session, Protocol::C2S_TravelLevel& pkt)
{
	return false;
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

bool Handle_C2S_Test(PacketSessionPtr& session, Protocol::C2S_Test& pkt)
{
	bool ret = false;

	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	ret = playerState->IsValid();
	if (false == ret)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(playerState->GetSessionManager());
	if (gameState == nullptr)
	{
		return false;
	}

	const int32 ivalue			= pkt.i_value();
	const std::string svalue	= pkt.s_value();
	const int64 timestamp		= pkt.time_stamp();

	LoginRoomPtr&	room = gameState->GetRoom();
	RemotePlayerPtr remotePlayer = playerState->mRemotePlayer;
	
	const int64		Priority = playerState->GetSessionManager()->GetServiceTimeStamp();

	return true;
}

bool Handle_C2S_GetRoundTripTime(PacketSessionPtr& session, Protocol::C2S_GetRoundTripTime& pkt)
{

	const int64 serviceTimeStamp = session->GetSessionManager()->GetServiceTimeStamp();
	const int64 halfRTT = session->GetRoundTripTime();
	
	const int64 syncTimeStamp = serviceTimeStamp + halfRTT;

	Protocol::S2C_GetRoundTripTime roundTripTimePacket;
	roundTripTimePacket.set_time_stamp(syncTimeStamp);

	wprintf(L"SYNC TIMESTAMP : Server(%lld) Sync(%lld) RTT(%lld)\n", serviceTimeStamp, syncTimeStamp, halfRTT);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, roundTripTimePacket);
	session->Send(sendBuffer);

	return true;
}