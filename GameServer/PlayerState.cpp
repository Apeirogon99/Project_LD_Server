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

RemotePlayerPtr& PlayerState::GetRemotePlayer()
{
	return mRemotePlayer;
}
