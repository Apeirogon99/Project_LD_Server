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
	S2C_PlayerEndAutoAttack = 2015,
	S2C_UpdateExperience = 2016,
	S2C_LevelUp = 2017,
	S2C_DeathPlayer = 2018,
	C2S_RevivePlayer = 2019,
	S2C_RevivePlayer = 2020,
	C2S_Chat = 2021,
	S2C_Chat = 2022,
	C2S_LoadFriendList = 2023,
	S2C_LoadFriendList = 2024,
	C2S_RequestFriend = 2025,
	S2C_RequestFriend = 2026,
	C2S_BlockFriend = 2027,
	S2C_BlockFriend = 2028,
	S2C_ConnectFriend = 2029,
	S2C_DisConnectFriend = 2030,
	C2S_CreateParty = 2031,
	S2C_CreateParty = 2032,
	C2S_RequestEnterParty = 2033,
	S2C_RequestEnterParty = 2034,
	S2C_RequestParty = 2035,
	C2S_RequestLeaveParty = 2036,
	S2C_RequestLeaveParty = 2037,
	C2S_RequestLeaderParty = 2038,
	S2C_RequestLeaderParty = 2039,
	C2S_ResponeParty = 2040,
	S2C_ResponeParty = 2041,
	S2C_LoadParty = 2042,
	S2C_NotifyParty = 2043,
	S2C_EnterPartyPlayer = 2044,
	S2C_LeavePartyPlayer = 2045,
	S2C_AppearItem = 2046,
	S2C_AppearArrow = 2047,
	S2C_AppearProtal = 2048,
	S2C_AppearObstruction = 2049,
	C2S_InteractiveObject = 2050,
	S2C_MovementProjectile = 2051,
	S2C_Teleport = 2052,
	S2C_AppearEnemy = 2053,
	S2C_DetectChangeEnemy = 2054,
	S2C_MovementEnemy = 2055,
	S2C_AnimationMovementEnemy = 2056,
	S2C_EnemyAutoAttack = 2057,
	S2C_HitEnemy = 2058,
	S2C_DeathEnemy = 2059,
	S2C_EnemyTeleport = 2060,
	S2C_DisAppearGameObject = 2061,
	C2S_LoadInventory = 2062,
	S2C_LoadInventory = 2063,
	C2S_InsertInventory = 2064,
	S2C_InsertInventory = 2065,
	C2S_UpdateInventory = 2066,
	S2C_UpdateInventory = 2067,
	C2S_DeleteInventory = 2068,
	S2C_DeleteInventory = 2069,
	S2C_RollbackInventory = 2070,
	C2S_ReplaceEqipment = 2071,
	S2C_ReplaceEqipment = 2072,
	S2C_LoadSkillTree = 2073,
	C2S_UpdateSkillTree = 2074,
	S2C_UpdateSkillTree = 2075,
	C2S_SetUseKeyAction = 2076,
	S2C_SetUseKeyAction = 2077,
	C2S_PressedUseKeyAction = 2078,
	C2S_ReleaseUseKeyAction = 2079,
	S2C_ResponseUseKeyAction = 2080,
	S2C_AppearSkill = 2081,
	S2C_ReactionSkill = 2082,
	S2C_EndReactionSkill = 2083,
	S2C_SkillCoolTime = 2084,
	S2C_PushBuff = 2085,
	S2C_ReleaseBuff = 2086,
	S2C_DebugBox = 2087,
	S2C_DebugCircle = 2088,
	C2S_RequestEnterDungeon = 2089,
	S2C_RequestEnterDungeon = 2090,
	S2C_ResponseEnterDungeon = 2091,
	C2S_CompleteLoadDungeon = 2092,
	S2C_WaitingLoadDungeon = 2093,
	S2C_CompleteLoadDungeon = 2094,
	S2C_EnterPortal = 2095,
	S2C_LeavePortal = 2096,
	S2C_PlaySequence = 2097,
	C2S_SkipSequence = 2098,
	S2C_SkipSequence = 2099,
	S2C_EndSequence = 2100,
	C2S_StartPack = 2101,
	S2C_StartPack = 2102,
	S2C_ApeearAttackTestUnit = 2103,
	S2C_MovementAttackTestUnit = 2104,
	S2C_BeginOverlapAttackTestUnit = 2105,
	S2C_EndOverlapAttackTestUnit = 2106,
};
*/

// Custom Handlers
bool Handle_C2S_EnterGameServer(PacketSessionPtr& session, Protocol::C2S_EnterGameServer& pkt);
bool Handle_C2S_LeaveGameServer(PacketSessionPtr& session, Protocol::C2S_LeaveGameServer& pkt);
bool Handle_C2S_Tick(PacketSessionPtr& session, Protocol::C2S_Tick& pkt);
bool Handle_C2S_MovementCharacter(PacketSessionPtr& session, Protocol::C2S_MovementCharacter& pkt);
bool Handle_C2S_PlayAnimation(PacketSessionPtr& session, Protocol::C2S_PlayAnimation& pkt);
bool Handle_C2S_PlayerAutoAttack(PacketSessionPtr& session, Protocol::C2S_PlayerAutoAttack& pkt);
bool Handle_C2S_RevivePlayer(PacketSessionPtr& session, Protocol::C2S_RevivePlayer& pkt);
bool Handle_C2S_Chat(PacketSessionPtr& session, Protocol::C2S_Chat& pkt);
bool Handle_C2S_LoadFriendList(PacketSessionPtr& session, Protocol::C2S_LoadFriendList& pkt);
bool Handle_C2S_RequestFriend(PacketSessionPtr& session, Protocol::C2S_RequestFriend& pkt);
bool Handle_C2S_BlockFriend(PacketSessionPtr& session, Protocol::C2S_BlockFriend& pkt);
bool Handle_C2S_CreateParty(PacketSessionPtr& session, Protocol::C2S_CreateParty& pkt);
bool Handle_C2S_RequestEnterParty(PacketSessionPtr& session, Protocol::C2S_RequestEnterParty& pkt);
bool Handle_C2S_RequestLeaveParty(PacketSessionPtr& session, Protocol::C2S_RequestLeaveParty& pkt);
bool Handle_C2S_RequestLeaderParty(PacketSessionPtr& session, Protocol::C2S_RequestLeaderParty& pkt);
bool Handle_C2S_ResponeParty(PacketSessionPtr& session, Protocol::C2S_ResponeParty& pkt);
bool Handle_C2S_InteractiveObject(PacketSessionPtr& session, Protocol::C2S_InteractiveObject& pkt);
bool Handle_C2S_LoadInventory(PacketSessionPtr& session, Protocol::C2S_LoadInventory& pkt);
bool Handle_C2S_InsertInventory(PacketSessionPtr& session, Protocol::C2S_InsertInventory& pkt);
bool Handle_C2S_UpdateInventory(PacketSessionPtr& session, Protocol::C2S_UpdateInventory& pkt);
bool Handle_C2S_DeleteInventory(PacketSessionPtr& session, Protocol::C2S_DeleteInventory& pkt);
bool Handle_C2S_ReplaceEqipment(PacketSessionPtr& session, Protocol::C2S_ReplaceEqipment& pkt);
bool Handle_C2S_UpdateSkillTree(PacketSessionPtr& session, Protocol::C2S_UpdateSkillTree& pkt);
bool Handle_C2S_SetUseKeyAction(PacketSessionPtr& session, Protocol::C2S_SetUseKeyAction& pkt);
bool Handle_C2S_PressedUseKeyAction(PacketSessionPtr& session, Protocol::C2S_PressedUseKeyAction& pkt);
bool Handle_C2S_ReleaseUseKeyAction(PacketSessionPtr& session, Protocol::C2S_ReleaseUseKeyAction& pkt);
bool Handle_C2S_RequestEnterDungeon(PacketSessionPtr& session, Protocol::C2S_RequestEnterDungeon& pkt);
bool Handle_C2S_CompleteLoadDungeon(PacketSessionPtr& session, Protocol::C2S_CompleteLoadDungeon& pkt);
bool Handle_C2S_SkipSequence(PacketSessionPtr& session, Protocol::C2S_SkipSequence& pkt);
bool Handle_C2S_StartPack(PacketSessionPtr& session, Protocol::C2S_StartPack& pkt);

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
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_RevivePlayer)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_RevivePlayer>(Handle_C2S_RevivePlayer, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_Chat)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_Chat>(Handle_C2S_Chat, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_LoadFriendList)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_LoadFriendList>(Handle_C2S_LoadFriendList, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_RequestFriend)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_RequestFriend>(Handle_C2S_RequestFriend, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_BlockFriend)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_BlockFriend>(Handle_C2S_BlockFriend, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_CreateParty)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_CreateParty>(Handle_C2S_CreateParty, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_RequestEnterParty)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_RequestEnterParty>(Handle_C2S_RequestEnterParty, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_RequestLeaveParty)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_RequestLeaveParty>(Handle_C2S_RequestLeaveParty, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_RequestLeaderParty)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_RequestLeaderParty>(Handle_C2S_RequestLeaderParty, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_ResponeParty)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_ResponeParty>(Handle_C2S_ResponeParty, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_InteractiveObject)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_InteractiveObject>(Handle_C2S_InteractiveObject, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_LoadInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_LoadInventory>(Handle_C2S_LoadInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_InsertInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_InsertInventory>(Handle_C2S_InsertInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_UpdateInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_UpdateInventory>(Handle_C2S_UpdateInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_DeleteInventory)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_DeleteInventory>(Handle_C2S_DeleteInventory, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_ReplaceEqipment)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_ReplaceEqipment>(Handle_C2S_ReplaceEqipment, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_UpdateSkillTree)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_UpdateSkillTree>(Handle_C2S_UpdateSkillTree, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_SetUseKeyAction)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_SetUseKeyAction>(Handle_C2S_SetUseKeyAction, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_PressedUseKeyAction)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_PressedUseKeyAction>(Handle_C2S_PressedUseKeyAction, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_ReleaseUseKeyAction)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_ReleaseUseKeyAction>(Handle_C2S_ReleaseUseKeyAction, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_RequestEnterDungeon)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_RequestEnterDungeon>(Handle_C2S_RequestEnterDungeon, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_CompleteLoadDungeon)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_CompleteLoadDungeon>(Handle_C2S_CompleteLoadDungeon, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_SkipSequence)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_SkipSequence>(Handle_C2S_SkipSequence, session, buffer, len); };
		inPacketFunc[static_cast<uint16>(EPakcetID::C2S_StartPack)] = [](PacketSessionPtr& session, BYTE* buffer, int32 len) { return PacketHandler::HandlePacket<Protocol::C2S_StartPack>(Handle_C2S_StartPack, session, buffer, len); };
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
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_PlayerEndAutoAttack& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_PlayerEndAutoAttack)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_UpdateExperience& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_UpdateExperience)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LevelUp& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LevelUp)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DeathPlayer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DeathPlayer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RevivePlayer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RevivePlayer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_Chat& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_Chat)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LoadFriendList& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LoadFriendList)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RequestFriend& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RequestFriend)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_BlockFriend& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_BlockFriend)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ConnectFriend& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ConnectFriend)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DisConnectFriend& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DisConnectFriend)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_CreateParty& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_CreateParty)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RequestEnterParty& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RequestEnterParty)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RequestParty& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RequestParty)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RequestLeaveParty& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RequestLeaveParty)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RequestLeaderParty& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RequestLeaderParty)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ResponeParty& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ResponeParty)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LoadParty& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LoadParty)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_NotifyParty& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_NotifyParty)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EnterPartyPlayer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnterPartyPlayer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LeavePartyPlayer& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LeavePartyPlayer)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearItem& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearItem)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearArrow& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearArrow)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearProtal& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearProtal)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearObstruction& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearObstruction)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_MovementProjectile& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_MovementProjectile)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_Teleport& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_Teleport)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DetectChangeEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DetectChangeEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_MovementEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_MovementEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AnimationMovementEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AnimationMovementEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EnemyAutoAttack& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnemyAutoAttack)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_HitEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_HitEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DeathEnemy& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DeathEnemy)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EnemyTeleport& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnemyTeleport)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DisAppearGameObject& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DisAppearGameObject)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LoadInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LoadInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_InsertInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_InsertInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_UpdateInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_UpdateInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DeleteInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DeleteInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RollbackInventory& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RollbackInventory)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ReplaceEqipment& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ReplaceEqipment)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LoadSkillTree& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LoadSkillTree)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_UpdateSkillTree& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_UpdateSkillTree)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_SetUseKeyAction& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_SetUseKeyAction)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ResponseUseKeyAction& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ResponseUseKeyAction)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_AppearSkill& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_AppearSkill)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ReactionSkill& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ReactionSkill)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EndReactionSkill& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EndReactionSkill)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_SkillCoolTime& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_SkillCoolTime)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_PushBuff& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_PushBuff)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ReleaseBuff& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ReleaseBuff)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DebugBox& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DebugBox)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_DebugCircle& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_DebugCircle)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_RequestEnterDungeon& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_RequestEnterDungeon)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ResponseEnterDungeon& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ResponseEnterDungeon)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_WaitingLoadDungeon& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_WaitingLoadDungeon)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_CompleteLoadDungeon& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_CompleteLoadDungeon)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EnterPortal& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EnterPortal)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_LeavePortal& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_LeavePortal)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_PlaySequence& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_PlaySequence)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_SkipSequence& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_SkipSequence)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EndSequence& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EndSequence)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_StartPack& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_StartPack)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_ApeearAttackTestUnit& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_ApeearAttackTestUnit)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_MovementAttackTestUnit& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_MovementAttackTestUnit)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_BeginOverlapAttackTestUnit& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_BeginOverlapAttackTestUnit)); }
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, Protocol::S2C_EndOverlapAttackTestUnit& pkt) { return PacketHandler::MakeSendBuffer(session, pkt, static_cast<uint16>(EPakcetID::S2C_EndOverlapAttackTestUnit)); }

};