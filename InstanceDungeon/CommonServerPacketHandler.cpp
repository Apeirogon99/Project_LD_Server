#include "pch.h"
#include "CommonServerPacketHandler.h"

bool Handle_C2S_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::C2S_ReplicatedServerTimeStamp& pkt)
{
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