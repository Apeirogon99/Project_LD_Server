#pragma once
#include "PacketHandler.h"

class PacketManager : public PacketHandler
{
public:
	static void Init();

private:
	friend class GameServerPacketHandler;
	friend class CommonServerPacketHandler;
};

