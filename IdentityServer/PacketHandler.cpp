#include "pch.h"
#include "PacketHandler.h"

#include "IdentityServerPacketHandler.h"

PacketHandlerFunc PacketHandler::PacketHandlers[UINT16_MAX];

void PacketHandler::Init()
{
	for (int i = 0; i < UINT16_MAX; ++i)
	{
		PacketHandlers[i] = Handle_INVALID;
	}

	IdentityServerPacketHandler::Init(PacketHandlers);

	wprintf(L"[PacketHandler::Init] : Successful init packet handls\n");
}
