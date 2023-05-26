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

	S2C_ReplicatedServerTimeStamp = 9000,
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
	static SendBufferPtr MakeSendBuffer(PacketSessionPtr& session, T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SessionManagerPtr manager = session->GetSessionManager();
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
	friend class IdentityServerPacketHandler;
	friend class CommonServerPacketHandler;

public:
	static PacketHandlerFunc PacketHandlers[UINT16_MAX];
};

