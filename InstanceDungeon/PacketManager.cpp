#include "pch.h"
#include "PacketManager.h"

#include <GameServerPacketHandler.h>
#include "DungeonServerPacketHandler.h"

void PacketManager::Init()
{
	for (int32 i = 0; i < UINT16_MAX; ++i)
	{
		PacketHandlers[i] = Handle_INVALID;
	}

	DungeonServerPacketHandler::Init(PacketHandlers);
	CommonServerPacketHandler::Init(PacketHandlers);

	wprintf(L"[PacketManager::Init] : Successful init packet handls\n");
}
