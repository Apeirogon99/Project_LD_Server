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
	C2S_AttackToEnemy = 2012,
	S2C_AttackToEnemy = 2013,
	S2C_AppearItem = 2014,
	S2C_AppearEnemy = 2015,
	S2C_TickEnemy = 2016,
	S2C_MovementEnemy = 2017,
	S2C_AttackToPlayer = 2018,
	S2C_TargetingToPlayer = 2019,
	S2C_HitEnemy = 2020,
	S2C_DeathEnemy = 2021,
	S2C_DisAppearGameObject = 2022,
	C2S_LoadInventory = 2023,
	S2C_LoadInventory = 2024,
	C2S_InsertInventory = 2025,
	S2C_InsertInventory = 2026,
	C2S_UpdateInventory = 2027,
	S2C_UpdateInventory = 2028,
	C2S_DeleteInventory = 2029,
	S2C_DeleteInventory = 2030,
	S2C_RollbackInventory = 2031,
	C2S_ReplaceEqipment = 2032,
	S2C_ReplaceEqipment = 2033,

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

