#include "pch.h"
#include "PlayerState.h"

PlayerState::PlayerState() : mRemotePlayer(nullptr)
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

	GameTaskPtr task = gameState->GetGameTask();
	if (nullptr == task)
	{
		return;
	}

	WorldPtr world = task->GetWorld();
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
		this->PlayerStateLog(L"PlayerState::OnRecvPacket() : Failed to handle packet\n");
		return;
	}
}

void PlayerState::BroadcastMonitors(SendBufferPtr inSendBuffer)
{
	Monitors& monitors = GetMonitors();
	for (auto monitor : monitors)
	{
		PlayerStatePtr playerState = monitor->GetPlayerState().lock();
		if (nullptr != playerState)
		{
			playerState->Send(inSendBuffer);
		}
	}
}
