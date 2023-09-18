#include "pch.h"
#include "CommonServerPacketHandler.h"

bool Handle_C2S_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::C2S_ReplicatedServerTimeStamp& pkt)
{
	const int64 serviceTimeStamp = session->GetSessionManager()->GetServiceTimeStamp();
	const int64 serverUtcTimeStmap = Time::GetUTCTime();

	const int64 clientUtcTimeStamp = pkt.utc_time();
	const int64 rtt = pkt.rtt();

	session->GetRoundTripTime().AddLatency(rtt);

	Protocol::S2C_ReplicatedServerTimeStamp packet;
	packet.set_utc_time(clientUtcTimeStamp);
	packet.set_time_stamp(serviceTimeStamp);

	//wprintf(L"[RTT:%lld] [SERVER::%lld] [CLIENT::%lld] [DIFF::%lld]\n", session->GetRoundTripTime().GetRoundTripTime(), serverUtcTimeStmap, clientUtcTimeStamp, serverUtcTimeStmap - clientUtcTimeStamp);

	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(session, packet);
	session->DirectSend(sendBuffer);
	return true;
}

bool Handle_S2C_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::S2C_ReplicatedServerTimeStamp& pkt)
{
	return false;
}

bool Handle_C2S_TravelLevel(PacketSessionPtr& session, Protocol::C2S_TravelLevel& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	Protocol::S2C_TravelLevel packet;
	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(session, packet);
	session->Send(sendBuffer);
	return true;
}

bool Handle_S2C_TravelLevel(PacketSessionPtr& session, Protocol::S2C_TravelLevel& pkt)
{
	return false;
}

bool Handle_C2S_TravelServer(PacketSessionPtr& session, Protocol::C2S_TravelServer& pkt)
{
	return true;
}

bool Handle_S2C_TravelServer(PacketSessionPtr& session, Protocol::S2C_TravelServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	if (nullptr == playerState)
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

	LoginWorldPtr world = task->GetWorld();
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
	bool ret = room->PushTask(priority, &SelectRoom::StartCharacterRespone, playerState, pkt);
	if (false == ret)
	{
		return false;
	}

	return false;
}

bool Handle_C2S_StateServer(PacketSessionPtr& session, Protocol::C2S_StateServer& pkt)
{
	return true;
}

bool Handle_S2C_StateServer(PacketSessionPtr& session, Protocol::S2C_StateServer& pkt)
{
	return true;
}