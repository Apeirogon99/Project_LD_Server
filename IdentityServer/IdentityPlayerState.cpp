#include "pch.h"
#include "IdentityPlayerState.h"

IdentityPlayerState::IdentityPlayerState()
{
}

IdentityPlayerState::~IdentityPlayerState()
{
}

void IdentityPlayerState::OnConnected()
{

}

void IdentityPlayerState::OnSend(uint32 len)
{

}

void IdentityPlayerState::OnIcmp()
{
	const int64 serviceTimeStamp = this->GetSessionManager()->GetServiceTimeStamp();
	const int64 uctTimeStamp = Time::GetUTCTime();

	const int64 syncTimeStamp = serviceTimeStamp;

	Protocol::S2C_ReplicatedServerTimeStamp packet;
	packet.set_time_stamp(syncTimeStamp);
	packet.set_utc_time(uctTimeStamp);

	//std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 400 + 200));

	PacketSessionPtr session = this->GetPacketSessionRef();
	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(session, packet);
	session->Send(sendBuffer);
}

void IdentityPlayerState::OnDisconnected()
{
	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(this->GetSessionManager());
	const int64 serviceTime = gameState->GetServiceTimeStamp();
	gameState->GetRoom()->PushTask(serviceTime, &LoginRoom::Leave, std::static_pointer_cast<IdentityPlayerState>(shared_from_this()));
}

void IdentityPlayerState::OnRecvPacket(BYTE* buffer, const uint32 len)
{
	PacketSessionPtr session = GetPacketSessionRef();

	bool result = false;
	result = PacketHandler::HandlePacket(session, buffer, len);
	if (false == result)
	{
		this->SessionLog(L"IdentityPlayerState::OnRecvPacket() : Failed to handle packet\n");
		return;
	}
}