#include "pch.h"
#include "PacketHandler.h"

#include "GameServerPacketHandler.h"
#include "CommonServerPacketHandler.h"

PacketHandlerFunc PacketHandler::PacketHandlers[UINT16_MAX];

bool Handle_INVALID(PacketSessionPtr& session, BYTE* buffer, int32 len)
{
	return false;
}

void PacketHandler::Init()
{
	for (int32 i = 0; i < UINT16_MAX; ++i)
	{
		PacketHandlers[i] = Handle_INVALID;
	}

	GameServerPacketHandler::Init(PacketHandlers);
	CommonServerPacketHandler::Init(PacketHandlers);

	wprintf(L"[PacketHandler::Init] : Successful init packet handls\n");
}