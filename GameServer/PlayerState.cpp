#include "pch.h"
#include "PlayerState.h"

PlayerState::PlayerState()
{
}

PlayerState::~PlayerState()
{
}

void PlayerState::OnConnected()
{
}

void PlayerState::OnSend(uint32 len)
{
}

void PlayerState::OnIcmp()
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

void PlayerState::OnDisconnected()
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(GetPacketSessionRef());
	if (nullptr == playerState)
	{
		return;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return;
	}

	WorldPtr world = gameState->GetWorld();
	if (nullptr == world)
	{
		return;
	}

	const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &World::Leave, playerState);
}

void PlayerState::OnRecvPacket(BYTE* buffer, const uint32 len)
{
	PacketSessionPtr session = GetPacketSessionRef();

	bool result = false;
	result = PacketHandler::HandlePacket(session, buffer, len);
	if (false == result)
	{
		this->PlayerStateLog(L"IdentityPlayerState::OnRecvPacket() : Failed to handle packet\n");
		return;
	}
}

PlayerStateRef PlayerState::GetPlayerStateRef()
{
	return std::static_pointer_cast<PlayerState>(shared_from_this());
}

RemotePlayerPtr& PlayerState::GetRemotePlayer()
{
	return mRemotePlayer;
}
