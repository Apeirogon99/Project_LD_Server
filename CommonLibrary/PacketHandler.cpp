#include "pch.h"
#include "PacketHandler.h"

PacketHandlerFunc PacketHandler::PacketHandlers[UINT16_MAX];

bool Handle_INVALID(PacketSessionPtr& session, BYTE* buffer, int32 len)
{
	return false;
}