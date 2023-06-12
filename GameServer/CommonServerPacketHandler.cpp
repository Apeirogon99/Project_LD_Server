#include "pch.h"
#include "CommonServerPacketHandler.h"

bool Handle_C2S_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::C2S_ReplicatedServerTimeStamp& pkt)
{
	const int64 serviceTimeStamp		= session->GetSessionManager()->GetServiceTimeStamp();
	const int64 serverUctTimeStamp		= Time::GetUTCTime();
	const int64 clientUtcTimeStamp		= pkt.utc_time();
	//const int64 rtt						= serverUctTimeStamp - clientUtcTimeStamp;
	const int64 rtt = 0;

	Protocol::S2C_ReplicatedServerTimeStamp packet;
	packet.set_time_stamp(serviceTimeStamp);
	packet.set_utc_time(serverUctTimeStamp);
	packet.set_rtt(rtt);

	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(session, packet);
	session->Send(sendBuffer);
	return true;
}

bool Handle_C2S_TravelServer(PacketSessionPtr& session, Protocol::C2S_TravelServer& pkt)
{
	return true;
}