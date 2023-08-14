#pragma once

using PacketHandlerFunc = std::function<bool(PacketSessionPtr&, BYTE*, int32)>;

enum class EPakcetID : uint16
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
	S2C_MovementProjectile = 2044,
	S2C_AppearEnemy = 2045,
	S2C_DetectChangeEnemy = 2046,
	S2C_MovementEnemy = 2047,
	S2C_EnemyAutoAttack = 2048,
	S2C_HitEnemy = 2049,
	S2C_DeathEnemy = 2050,
	S2C_DisAppearGameObject = 2051,
	C2S_LoadInventory = 2052,
	S2C_LoadInventory = 2053,
	C2S_InsertInventory = 2054,
	S2C_InsertInventory = 2055,
	C2S_UpdateInventory = 2056,
	S2C_UpdateInventory = 2057,
	C2S_DeleteInventory = 2058,
	S2C_DeleteInventory = 2059,
	S2C_RollbackInventory = 2060,
	C2S_ReplaceEqipment = 2061,
	S2C_ReplaceEqipment = 2062,
	S2C_LoadSkillTree = 2063,
	C2S_UpdateSkillTree = 2064,
	S2C_UpdateSkillTree = 2065,
	S2C_AppearBuff = 2066,

	C2S_ReplicatedServerTimeStamp = 9000,
	S2C_ReplicatedServerTimeStamp = 9001,
	C2S_TravelLevel = 9002,
	S2C_TravelLevel = 9003,
	C2S_TravelServer = 9004,
	S2C_TravelServer = 9005,
};

bool Handle_INVALID(PacketSessionPtr& session, BYTE* buffer, int32 len);

class PacketHandler
{
public:
	static void Init();

	static bool HandlePacket(PacketSessionPtr& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return PacketHandlers[header->id](session, buffer, len);
	}

private:
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

		//SessionManagerPtr manager = session->GetSessionManager();
		//SendRingBuffer& sendRingBuffer = manager->GetSendRingBuffer();
		//SendBufferPtr sendBuffer = sendRingBuffer.Writer(packetSize);
		SendBufferPtr sendBuffer = std::make_shared<SendBuffer>(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		//sendRingBuffer.Reserve(packetSize);

		return sendBuffer;
	}

private:
	friend class GameServerPacketHandler;
	friend class CommonServerPacketHandler;

public:
	static PacketHandlerFunc PacketHandlers[UINT16_MAX];
};

