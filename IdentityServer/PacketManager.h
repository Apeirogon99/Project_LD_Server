#pragma once
#include "PacketHandler.h"

class PacketManager : public PacketHandler
{
public:
	static void Init();

private:
	friend class IdentityServerPacketHandler;
	friend class CommonServerPacketHandler;

};