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
	C2S_Chat = 2015,
	S2C_Chat = 2016,
	C2S_LoadFriendList = 2017,
	S2C_LoadFriendList = 2018,
	C2S_RequestFriend = 2019,
	S2C_RequestFriend = 2020,
	C2S_BlockFriend = 2021,
	S2C_BlockFriend = 2022,
	S2C_ConnectFriend = 2023,
	S2C_DisConnectFriend = 2024,
	S2C_AppearItem = 2025,
	S2C_AppearArrow = 2026,
	S2C_MovementProjectile = 2027,
	S2C_AppearEnemy = 2028,
	S2C_DetectChangeEnemy = 2029,
	S2C_MovementEnemy = 2030,
	S2C_EnemyAutoAttack = 2031,
	S2C_HitEnemy = 2032,
	S2C_DeathEnemy = 2033,
	S2C_DisAppearGameObject = 2034,
	C2S_LoadInventory = 2035,
	S2C_LoadInventory = 2036,
	C2S_InsertInventory = 2037,
	S2C_InsertInventory = 2038,
	C2S_UpdateInventory = 2039,
	S2C_UpdateInventory = 2040,
	C2S_DeleteInventory = 2041,
	S2C_DeleteInventory = 2042,
	S2C_RollbackInventory = 2043,
	C2S_ReplaceEqipment = 2044,
	S2C_ReplaceEqipment = 2045,

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

