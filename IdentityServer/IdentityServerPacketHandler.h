#pragma once

/*
enum class EPakcetID: uint16
{
	C2S_EnterIdentityServer = 1000,
	S2C_EnterIdentityServer = 1001,
	C2S_LeaveIdentityServer = 1002,
	S2C_LeaveIdentityServer = 1003,
	C2S_Singin = 1004,
	S2C_Singin = 1005,
	C2S_Singup = 1006,
	S2C_Singup = 1007,
	C2S_EmailVerified = 1008,
	S2C_EmailVerified = 1009,
	S2C_LoadServer = 1010,
	C2S_LoadCharacters = 1011,
	S2C_LoadCharacters = 1012,
	C2S_CreateCharacter = 1013,
	S2C_CreateCharacter = 1014,
	C2S_DeleteCharacter = 1015,
	S2C_DeleteCharacter = 1016,
	C2S_SelectServer = 1017,
	C2S_TravelServer = 1018,
	S2C_TravelServer = 1019,
};
*/

// Custom Handlers
bool Handle_C2S_EnterIdentityServer(PacketSessionPtr& session, Protocol::C2S_EnterIdentityServer& pkt);
bool Handle_C2S_LeaveIdentityServer(PacketSessionPtr& session, Protocol::C2S_LeaveIdentityServer& pkt);
bool Handle_C2S_Singin(PacketSessionPtr& session, Protocol::C2S_Singin& pkt);
bool Handle_C2S_Singup(PacketSessionPtr& session, Protocol::C2S_Singup& pkt);
bool Handle_C2S_EmailVerified(PacketSessionPtr& session, Protocol::C2S_EmailVerified& pkt);
bool Handle_C2S_LoadCharacters(PacketSessionPtr& session, Protocol::C2S_LoadCharacters& pkt);
bool Handle_C2S_CreateCharacter(PacketSessionPtr& session, Protocol::C2S_CreateCharacter& pkt);
bool Handle_C2S_DeleteCharacter(PacketSessionPtr& session, Protocol::C2S_DeleteCharacter& pkt);
bool Handle_C2S_SelectServer(PacketSessionPtr& session, Protocol::C2S_SelectServer& pkt);
bool Handle_C2S_TravelServer(PacketSessionPtr& session, Protocol::C2S_TravelServer& pkt);

class IdentityServerPacketHandler
{
public:
	static void Init(PacketHandlerFunc inPacketFunc[UINT16_MAX])
	{
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_EnterIdentityServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_EnterIdentityServer>(Handle_C2S_EnterIdentityServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_LeaveIdentityServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_LeaveIdentityServer>(Handle_C2S_LeaveIdentityServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_Singin)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_Singin>(Handle_C2S_Singin, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_Singup)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_Singup>(Handle_C2S_Singup, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_EmailVerified)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_EmailVerified>(Handle_C2S_EmailVerified, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_LoadCharacters)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_LoadCharacters>(Handle_C2S_LoadCharacters, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_CreateCharacter)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_CreateCharacter>(Handle_C2S_CreateCharacter, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_DeleteCharacter)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_DeleteCharacter>(Handle_C2S_DeleteCharacter, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_SelectServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_SelectServer>(Handle_C2S_SelectServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_TravelServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_TravelServer>(Handle_C2S_TravelServer, session, buffer, len); };
	}
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_EnterIdentityServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnterIdentityServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_LeaveIdentityServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LeaveIdentityServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_Singin& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_Singin)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_Singup& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_Singup)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_EmailVerified& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EmailVerified)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_LoadServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LoadServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_LoadCharacters& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LoadCharacters)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_CreateCharacter& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_CreateCharacter)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_DeleteCharacter& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DeleteCharacter)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_TravelServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_TravelServer)); }

};