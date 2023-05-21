#include "pch.h"
#include "GameServerPacketHandler.h"

bool Handle_C2S_EnterGameServer(PacketSessionPtr& session, Protocol::C2S_EnterGameServer& pkt)
{
	return false;
}

bool Handle_C2S_LeaveGameServer(PacketSessionPtr& session, Protocol::C2S_LeaveGameServer& pkt)
{
	return false;
}

bool Handle_C2S_MovementCharacter(PacketSessionPtr& session, Protocol::C2S_MovementCharacter& pkt)
{
	return false;
}
