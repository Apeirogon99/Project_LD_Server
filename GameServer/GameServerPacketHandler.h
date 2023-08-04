#pragma once

/*
enum class EPakcetID: uint16
{
	C2S_EnterGameServer = 2000,
	S2C_EnterGameServer = 2001,
	C2S_LeaveGameServer = 2002,
	S2C_LeaveGameServer = 2003,
	C2S_Tick = 2004,
	S2C_Tick = 2005,
	S2C_AppearCharacter = 2006,
	S2C_DisAppearCharacter = 2007,
	C2S_MovementCharacter = 2008,
	S2C_MovementCharacter = 2009,
	C2S_PlayAnimation = 2010,
	S2C_PlayAnimation = 2011,
	S2C_DetectChangePlayer = 2012,
	C2S_PlayerAutoAttack = 2013,
	S2C_PlayerAutoAttack = 2014,
	C2S_Chat = 2015,
	S2C_Chat = 2016,
	S2C_AppearItem = 2017,
	S2C_AppearArrow = 2018,
	S2C_MovementProjectile = 2019,
	S2C_AppearEnemy = 2020,
	S2C_DetectChangeEnemy = 2021,
	S2C_MovementEnemy = 2022,
	S2C_EnemyAutoAttack = 2023,
	S2C_HitEnemy = 2024,
	S2C_DeathEnemy = 2025,
	S2C_DisAppearGameObject = 2026,
	C2S_LoadInventory = 2027,
	S2C_LoadInventory = 2028,
	C2S_InsertInventory = 2029,
	S2C_InsertInventory = 2030,
	C2S_UpdateInventory = 2031,
	S2C_UpdateInventory = 2032,
	C2S_DeleteInventory = 2033,
	S2C_DeleteInventory = 2034,
	S2C_RollbackInventory = 2035,
	C2S_ReplaceEqipment = 2036,
	S2C_ReplaceEqipment = 2037,
};
*/

// Custom Handlers
bool Handle_C2S_EnterGameServer(PacketSessionPtr& session, Protocol::C2S_EnterGameServer& pkt);
bool Handle_C2S_LeaveGameServer(PacketSessionPtr& session, Protocol::C2S_LeaveGameServer& pkt);
bool Handle_C2S_Tick(PacketSessionPtr& session, Protocol::C2S_Tick& pkt);
bool Handle_C2S_MovementCharacter(PacketSessionPtr& session, Protocol::C2S_MovementCharacter& pkt);
bool Handle_C2S_PlayAnimation(PacketSessionPtr& session, Protocol::C2S_PlayAnimation& pkt);
bool Handle_C2S_PlayerAutoAttack(PacketSessionPtr& session, Protocol::C2S_PlayerAutoAttack& pkt);
bool Handle_C2S_Chat(PacketSessionPtr& session, Protocol::C2S_Chat& pkt);
bool Handle_C2S_LoadInventory(PacketSessionPtr& session, Protocol::C2S_LoadInventory& pkt);
bool Handle_C2S_InsertInventory(PacketSessionPtr& session, Protocol::C2S_InsertInventory& pkt);
bool Handle_C2S_UpdateInventory(PacketSessionPtr& session, Protocol::C2S_UpdateInventory& pkt);
bool Handle_C2S_DeleteInventory(PacketSessionPtr& session, Protocol::C2S_DeleteInventory& pkt);
bool Handle_C2S_ReplaceEqipment(PacketSessionPtr& session, Protocol::C2S_ReplaceEqipment& pkt);

class GameServerPacketHandler
{
public:
	static void Init(PacketHandlerFunc inPacketFunc[UINT16_MAX])
	{
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_EnterGameServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_EnterGameServer>(Handle_C2S_EnterGameServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_LeaveGameServer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_LeaveGameServer>(Handle_C2S_LeaveGameServer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_Tick)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_Tick>(Handle_C2S_Tick, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_MovementCharacter)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_MovementCharacter>(Handle_C2S_MovementCharacter, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_PlayAnimation)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_PlayAnimation>(Handle_C2S_PlayAnimation, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_PlayerAutoAttack)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_PlayerAutoAttack>(Handle_C2S_PlayerAutoAttack, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_Chat)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_Chat>(Handle_C2S_Chat, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_LoadInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_LoadInventory>(Handle_C2S_LoadInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_InsertInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_InsertInventory>(Handle_C2S_InsertInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_UpdateInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_UpdateInventory>(Handle_C2S_UpdateInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_DeleteInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_DeleteInventory>(Handle_C2S_DeleteInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_ReplaceEqipment)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_ReplaceEqipment>(Handle_C2S_ReplaceEqipment, session, buffer, len); };
	}
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EnterGameServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnterGameServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LeaveGameServer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LeaveGameServer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_Tick& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_Tick)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearCharacter& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearCharacter)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DisAppearCharacter& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DisAppearCharacter)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_MovementCharacter& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_MovementCharacter)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_PlayAnimation& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_PlayAnimation)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DetectChangePlayer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DetectChangePlayer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_PlayerAutoAttack& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_PlayerAutoAttack)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_Chat& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_Chat)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearItem& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearItem)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearArrow& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearArrow)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_MovementProjectile& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_MovementProjectile)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DetectChangeEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DetectChangeEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_MovementEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_MovementEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EnemyAutoAttack& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnemyAutoAttack)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_HitEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_HitEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DeathEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DeathEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DisAppearGameObject& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DisAppearGameObject)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LoadInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LoadInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_InsertInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_InsertInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_UpdateInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_UpdateInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DeleteInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DeleteInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RollbackInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RollbackInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ReplaceEqipment& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ReplaceEqipment)); }

};