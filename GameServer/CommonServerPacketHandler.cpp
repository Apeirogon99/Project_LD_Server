#include "pch.h"
#include "CommonServerPacketHandler.h"

bool Handle_C2S_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::C2S_ReplicatedServerTimeStamp& pkt)
{
	const int64 serviceTimeStamp = session->GetSessionManager()->GetServiceTimeStamp();
	const int64 serverUtcTimeStmap = Time::GetUTCTime();

	const int64 clientUtcTimeStamp	= pkt.utc_time();
	const int64 rtt					= pkt.rtt();

	session->GetRoundTripTime().AddLatency(rtt);

	Protocol::S2C_ReplicatedServerTimeStamp packet;
	packet.set_utc_time(clientUtcTimeStamp);
	packet.set_time_stamp(serviceTimeStamp);

	//wprintf(L"[RTT:%lld] [SERVER::%lld] [CLIENT::%lld] [DIFF::%f]\n", session->GetRoundTripTime().GetRoundTripTime(), serverUtcTimeStmap, clientUtcTimeStamp, static_cast<float>(serverUtcTimeStmap - clientUtcTimeStamp) / 1000.0f);

	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(session, packet);
	session->DirectSend(sendBuffer);
	return true;
}

bool Handle_S2C_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::S2C_ReplicatedServerTimeStamp& pkt)
{
	return true;
}

bool Handle_C2S_TravelLevel(PacketSessionPtr& session, Protocol::C2S_TravelLevel& pkt)
{
	return true;
}

bool Handle_S2C_TravelLevel(PacketSessionPtr& session, Protocol::S2C_TravelLevel& pkt)
{
	return true;
}

bool Handle_C2S_TravelServer(PacketSessionPtr& session, Protocol::C2S_TravelServer& pkt)
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

	GameWorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &GameWorld::ServerTravel, playerState, pkt);
	return true;
}

bool Handle_S2C_TravelServer(PacketSessionPtr& session, Protocol::S2C_TravelServer& pkt)
{
	return true;
}

bool Handle_C2S_StateServer(PacketSessionPtr& session, Protocol::C2S_StateServer& pkt)
{
	return true;
}

bool Handle_S2C_StateServer(PacketSessionPtr& session, Protocol::S2C_StateServer& pkt)
{
	return true;
}