#include "pch.h"
#include "CommonServerPacketHandler.h"

bool Handle_C2S_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::C2S_ReplicatedServerTimeStamp& pkt)
{
	const int64 serviceTimeStamp = session->GetSessionManager()->GetServiceTimeStamp();
	const int64 serverUctTimeStamp = Time::GetUTCTime();
	const int64 clientUtcTimeStamp = pkt.utc_time();
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

bool Handle_C2S_TravelLevel(PacketSessionPtr& session, Protocol::C2S_TravelLevel& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer || false == remotePlayer->IsValid())
	{
		return false;
	}

	const int32 prevLevel = static_cast<int32>(remotePlayer->GetRoomType());
	const int32 nextLevel = pkt.level();
	
	const int32 diffLevel = abs(prevLevel - nextLevel);

	if (diffLevel == 1)
	{
		remotePlayer->SetRoomType(static_cast<ERoomType>(nextLevel));
	}

	Protocol::S2C_TravelLevel packet;
	packet.set_error(diffLevel == 1 ? true : false);

	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(session, packet);
	session->Send(sendBuffer);
	return true;
}

bool Handle_C2S_TravelServer(PacketSessionPtr& session, Protocol::C2S_TravelServer& pkt)
{
	return true;
}