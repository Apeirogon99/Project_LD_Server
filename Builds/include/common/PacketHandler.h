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
	S2C_UpdateExperience = 2015,
	S2C_LevelUp = 2016,
	C2S_Chat = 2017,
	S2C_Chat = 2018,
	C2S_LoadFriendList = 2019,
	S2C_LoadFriendList = 2020,
	C2S_RequestFriend = 2021,
	S2C_RequestFriend = 2022,
	C2S_BlockFriend = 2023,
	S2C_BlockFriend = 2024,
	S2C_ConnectFriend = 2025,
	S2C_DisConnectFriend = 2026,
	C2S_CreateParty = 2027,
	S2C_CreateParty = 2028,
	C2S_RequestEnterParty = 2029,
	S2C_RequestEnterParty = 2030,
	S2C_RequestParty = 2031,
	C2S_RequestLeaveParty = 2032,
	S2C_RequestLeaveParty = 2033,
	C2S_RequestLeaderParty = 2034,
	S2C_RequestLeaderParty = 2035,
	C2S_ResponeParty = 2036,
	S2C_ResponeParty = 2037,
	S2C_LoadParty = 2038,
	S2C_NotifyParty = 2039,
	S2C_EnterPartyPlayer = 2040,
	S2C_LeavePartyPlayer = 2041,
	S2C_AppearItem = 2042,
	S2C_AppearArrow = 2043,
	S2C_AppearProtal = 2044,
	S2C_AppearObstruction = 2045,
	C2S_InteractiveObject = 2046,
	S2C_MovementProjectile = 2047,
	S2C_Teleport = 2048,
	S2C_AppearEnemy = 2049,
	S2C_DetectChangeEnemy = 2050,
	S2C_MovementEnemy = 2051,
	S2C_AnimationMovementEnemy = 2052,
	S2C_EnemyAutoAttack = 2053,
	S2C_HitEnemy = 2054,
	S2C_DeathEnemy = 2055,
	S2C_DisAppearGameObject = 2056,
	C2S_LoadInventory = 2057,
	S2C_LoadInventory = 2058,
	C2S_InsertInventory = 2059,
	S2C_InsertInventory = 2060,
	C2S_UpdateInventory = 2061,
	S2C_UpdateInventory = 2062,
	C2S_DeleteInventory = 2063,
	S2C_DeleteInventory = 2064,
	S2C_RollbackInventory = 2065,
	C2S_ReplaceEqipment = 2066,
	S2C_ReplaceEqipment = 2067,
	S2C_LoadSkillTree = 2068,
	C2S_UpdateSkillTree = 2069,
	S2C_UpdateSkillTree = 2070,
	C2S_SetUseKeyAction = 2071,
	S2C_SetUseKeyAction = 2072,
	C2S_PressedUseKeyAction = 2073,
	C2S_ReleaseUseKeyAction = 2074,
	S2C_AppearSkill = 2075,
	S2C_ReactionSkill = 2076,
	S2C_DebugBox = 2077,
	S2C_DebugCircle = 2078,
	C2S_RequestEnterDungeon = 2079,
	S2C_RequestEnterDungeon = 2080,
	S2C_ResponseEnterDungeon = 2081,
	C2S_CompleteLoadDungeon = 2082,
	S2C_WaitingLoadDungeon = 2083,
	S2C_CompleteLoadDungeon = 2084,
	S2C_EnterPortal = 2085,
	S2C_LeavePortal = 2086,

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

