#pragma once

using PacketHandlerFunc = std::function<bool(PacketSessionPtr&, BYTE*, int32)>;

enum class EPakcetID : uint16
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
	C2S_LoadServer = 1010,
	S2C_LoadServer = 1011,
	C2S_SelectServer = 1012,
	S2C_SelectServer = 1013,
	C2S_StartGame = 1014,
	S2C_StartGame = 1015,
	C2S_LoadCharacters = 1016,
	S2C_LoadCharacters = 1017,
	C2S_CreateCharacter = 1018,
	S2C_CreateCharacter = 1019,
	C2S_DeleteCharacter = 1020,
	S2C_DeleteCharacter = 1021,

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
	C2S_Chat = 2018,
	S2C_Chat = 2019,
	C2S_LoadFriendList = 2020,
	S2C_LoadFriendList = 2021,
	C2S_RequestFriend = 2022,
	S2C_RequestFriend = 2023,
	C2S_BlockFriend = 2024,
	S2C_BlockFriend = 2025,
	S2C_ConnectFriend = 2026,
	S2C_DisConnectFriend = 2027,
	C2S_CreateParty = 2028,
	S2C_CreateParty = 2029,
	C2S_RequestEnterParty = 2030,
	S2C_RequestEnterParty = 2031,
	S2C_RequestParty = 2032,
	C2S_RequestLeaveParty = 2033,
	S2C_RequestLeaveParty = 2034,
	C2S_RequestLeaderParty = 2035,
	S2C_RequestLeaderParty = 2036,
	C2S_ResponeParty = 2037,
	S2C_ResponeParty = 2038,
	S2C_LoadParty = 2039,
	S2C_NotifyParty = 2040,
	S2C_EnterPartyPlayer = 2041,
	S2C_LeavePartyPlayer = 2042,
	S2C_AppearItem = 2043,
	S2C_AppearArrow = 2044,
	S2C_AppearProtal = 2045,
	S2C_AppearObstruction = 2046,
	C2S_InteractiveObject = 2047,
	S2C_MovementProjectile = 2048,
	S2C_Teleport = 2049,
	S2C_AppearEnemy = 2050,
	S2C_DetectChangeEnemy = 2051,
	S2C_MovementEnemy = 2052,
	S2C_AnimationMovementEnemy = 2053,
	S2C_EnemyAutoAttack = 2054,
	S2C_HitEnemy = 2055,
	S2C_DeathEnemy = 2056,
	S2C_DisAppearGameObject = 2057,
	C2S_LoadInventory = 2058,
	S2C_LoadInventory = 2059,
	C2S_InsertInventory = 2060,
	S2C_InsertInventory = 2061,
	C2S_UpdateInventory = 2062,
	S2C_UpdateInventory = 2063,
	C2S_DeleteInventory = 2064,
	S2C_DeleteInventory = 2065,
	S2C_RollbackInventory = 2066,
	C2S_ReplaceEqipment = 2067,
	S2C_ReplaceEqipment = 2068,
	S2C_LoadSkillTree = 2069,
	C2S_UpdateSkillTree = 2070,
	S2C_UpdateSkillTree = 2071,
	C2S_SetUseKeyAction = 2072,
	S2C_SetUseKeyAction = 2073,
	C2S_PressedUseKeyAction = 2074,
	C2S_ReleaseUseKeyAction = 2075,
	S2C_ResponseUseKeyAction = 2076,
	S2C_AppearSkill = 2077,
	S2C_ReactionSkill = 2078,
	S2C_EndReactionSkill = 2079,
	S2C_SkillCoolTime = 2080,
	S2C_PushBuff = 2081,
	S2C_ReleaseBuff = 2082,
	S2C_DebugBox = 2083,
	S2C_DebugCircle = 2084,
	C2S_RequestEnterDungeon = 2085,
	S2C_RequestEnterDungeon = 2086,
	S2C_ResponseEnterDungeon = 2087,
	C2S_CompleteLoadDungeon = 2088,
	S2C_WaitingLoadDungeon = 2089,
	S2C_CompleteLoadDungeon = 2090,
	S2C_EnterPortal = 2091,
	S2C_LeavePortal = 2092,
	S2C_PlaySequence = 2093,
	C2S_SkipSequence = 2094,
	S2C_SkipSequence = 2095,
	S2C_EndSequence = 2096,
	C2S_StartPack = 2097,
	S2C_StartPack = 2098,

	C2S_EnterDungeon = 5000,
	S2C_EnterDungeon = 5001,

	C2S_ReplicatedServerTimeStamp = 9000,
	S2C_ReplicatedServerTimeStamp = 9001,
	C2S_TravelLevel = 9002,
	S2C_TravelLevel = 9003,
	C2S_TravelServer = 9004,
	S2C_TravelServer = 9005,
	C2S_StateServer = 9006,
	S2C_StateServer = 9007,
};

bool Handle_INVALID(PacketSessionPtr& session, BYTE* buffer, int32 len);

class PacketHandler
{
public:
	//static void Init();

	static bool HandlePacket(PacketSessionPtr& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return PacketHandlers[header->id](session, buffer, len);
	}

protected:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionPtr& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr session, T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferPtr sendBuffer = std::make_shared<SendBuffer>(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		return sendBuffer;
	}

protected:
	static PacketHandlerFunc PacketHandlers[UINT16_MAX];
};

