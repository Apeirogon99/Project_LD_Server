#pragma once

/*
enum class EPakcetID: uint16
{
	S2C_ReplicatedServerTimeStamp = 9000,
};
*/

// Custom Handlers

class CommonServerPacketHandler
{
public:
	static void Init(PacketHandlerFunc inPacketFunc[UINT16_MAX])
	{
	}
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_ReplicatedServerTimeStamp& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ReplicatedServerTimeStamp)); }

};