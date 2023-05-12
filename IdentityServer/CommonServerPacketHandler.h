#pragma once

enum class EPakcetID: uint16
{
	S2C_ReplicatedServerTimeStamp = 1000,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionPtr& session, BYTE* buffer, int32 len);

class CommonServerPacketHandler
{
public:
	static void Init(PacketHandlerFunc inPacketFunc[UINT16_MAX])
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			inPacketFunc[i] = Handle_INVALID;
	}
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_ReplicatedServerTimeStamp& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ReplicatedServerTimeStamp)); }

};