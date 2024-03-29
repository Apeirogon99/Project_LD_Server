#pragma once

/*
enum class EPakcetID: uint16
{
	C2S_ReplicatedServerTimeStamp = 9000,
	S2C_ReplicatedServerTimeStamp = 9001,
	C2S_TravelLevel = 9002,
	S2C_TravelLevel = 9003,
	C2S_TravelServer = 9004,
	S2C_TravelServer = 9005,
	C2S_StateServer = 9006,
	S2C_StateServer = 9007,
};
*/

// Custom Handlers
bool Handle_C2S_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::C2S_ReplicatedServerTimeStamp& pkt);
bool Handle_S2C_ReplicatedServerTimeStamp(PacketSessionPtr& session, Protocol::S2C_ReplicatedServerTimeStamp& pkt);
bool Handle_C2S_TravelLevel(PacketSessionPtr& session, Protocol::C2S_TravelLevel& pkt);
bool Handle_S2C_TravelLevel(PacketSessionPtr& session, Protocol::S2C_TravelLevel& pkt);
bool Handle_C2S_TravelServer(PacketSessionPtr& session, Protocol::C2S_TravelServer& pkt);
bool Handle_S2C_TravelServer(PacketSessionPtr& session, Protocol::S2C_TravelServer& pkt);
bool Handle_C2S_StateServer(PacketSessionPtr& session, Protocol::C2S_StateServer& pkt);
bool Handle_S2C_StateServer(PacketSessionPtr& session, Protocol::S2C_StateServer& pkt);

class CommonServerPacketHandler
{
public:
	static void Init(PacketHandlerFunc inPacketFunc[UINT16_MAX])
	{
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_ReplicatedServerTimeStamp)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_ReplicatedServerTimeStamp>(Handle_C2S_ReplicatedServerTimeStamp, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::S2C_ReplicatedServerTimeStamp)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::S2C_ReplicatedServerTimeStamp>(Handle_S2C_ReplicatedServerTimeStamp, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_TravelLevel)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_TravelLevel>(Handle_C2S_TravelLevel, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::S2C_TravelLevel)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::S2C_TravelLevel>(Handle_S2C_TravelLevel, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_TravelServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_TravelServer>(Handle_C2S_TravelServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::S2C_TravelServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::S2C_TravelServer>(Handle_S2C_TravelServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_StateServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_StateServer>(Handle_C2S_StateServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::S2C_StateServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::S2C_StateServer>(Handle_S2C_StateServer, session, buffer, len); };
	}
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::C2S_ReplicatedServerTimeStamp& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::C2S_ReplicatedServerTimeStamp)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ReplicatedServerTimeStamp& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ReplicatedServerTimeStamp)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::C2S_TravelLevel& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::C2S_TravelLevel)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_TravelLevel& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_TravelLevel)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::C2S_TravelServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::C2S_TravelServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_TravelServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_TravelServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::C2S_StateServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::C2S_StateServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_StateServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_StateServer)); }

};