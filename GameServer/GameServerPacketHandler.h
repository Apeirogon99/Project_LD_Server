#pragma once

/*
enum class EPakcetID: uint16
{
	C2S_EnterGameServer = 2000,
	S2C_EnterGameServer = 2001,
	C2S_LeaveGameServer = 2002,
	S2C_LeaveGameServer = 2003,
	S2C_AppearCharacter = 2004,
	S2C_DisAppearCharacter = 2005,
	C2S_MovementCharacter = 2006,
	S2C_MovementCharacter = 2007,
	C2S_InsertInventory = 2008,
	S2C_InsertInventory = 2009,
	C2S_UpdateInventory = 2010,
	S2C_UpdateInventory = 2011,
	C2S_DeleteInventory = 2012,
	S2C_DeleteInventory = 2013,
};
*/

// Custom Handlers
bool Handle_C2S_EnterGameServer(PacketSessionPtr& session, Protocol::C2S_EnterGameServer& pkt);
bool Handle_C2S_LeaveGameServer(PacketSessionPtr& session, Protocol::C2S_LeaveGameServer& pkt);
bool Handle_C2S_MovementCharacter(PacketSessionPtr& session, Protocol::C2S_MovementCharacter& pkt);
bool Handle_C2S_InsertInventory(PacketSessionPtr& session, Protocol::C2S_InsertInventory& pkt);
bool Handle_C2S_UpdateInventory(PacketSessionPtr& session, Protocol::C2S_UpdateInventory& pkt);
bool Handle_C2S_DeleteInventory(PacketSessionPtr& session, Protocol::C2S_DeleteInventory& pkt);

class GameServerPacketHandler
{
public:
	static void Init(PacketHandlerFunc inPacketFunc[UINT16_MAX])
	{
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_EnterGameServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_EnterGameServer>(Handle_C2S_EnterGameServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_LeaveGameServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_LeaveGameServer>(Handle_C2S_LeaveGameServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_MovementCharacter)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_MovementCharacter>(Handle_C2S_MovementCharacter, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_InsertInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_InsertInventory>(Handle_C2S_InsertInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_UpdateInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_UpdateInventory>(Handle_C2S_UpdateInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_DeleteInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_DeleteInventory>(Handle_C2S_DeleteInventory, session, buffer, len); };
	}
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_EnterGameServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnterGameServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_LeaveGameServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LeaveGameServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_AppearCharacter& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearCharacter)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_DisAppearCharacter& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DisAppearCharacter)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_MovementCharacter& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_MovementCharacter)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_InsertInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_InsertInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_UpdateInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_UpdateInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, Protocol::S2C_DeleteInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DeleteInventory)); }

};