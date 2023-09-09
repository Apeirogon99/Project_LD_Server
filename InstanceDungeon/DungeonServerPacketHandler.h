#pragma once

/*
enum class EPakcetID: uint16
{
	C2S_EnterDungeon = 5000,
	S2C_EnterDungeon = 5001,
};
*/

// Custom Handlers
bool Handle_C2S_EnterDungeon(PacketSessionPtr& session, Protocol::C2S_EnterDungeon& pkt);

class DungeonServerPacketHandler
{
public:
	static void Init(PacketHandlerFunc inPacketFunc[UINT16_MAX])
	{
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_EnterDungeon)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_EnterDungeon>(Handle_C2S_EnterDungeon, session, buffer, len); };
	}
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EnterDungeon& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnterDungeon)); }

};