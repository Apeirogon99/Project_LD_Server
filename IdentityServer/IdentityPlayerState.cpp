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

}

void IdentityPlayerState::OnDisconnected()
{
	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(this->GetSessionManager());
	if (gameState == nullptr)
	{
		return;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (task == nullptr)
	{
		return;
	}

	LoginWorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(this->GetRemotePlayer());
	if (remotePlayer == nullptr)
	{
		return;
	}

	if (false == world->IsValidPlayer(remotePlayer))
	{
		return;
	}

	const int64 Priority = gameState->GetServiceTimeStamp();
	bool ret = world->PushTask(Priority, &LoginWorld::LeaveWorld, std::static_pointer_cast<IdentityPlayerState>(this->GetRemoteClientRef().lock()));
	if (false == ret)
	{
		return;
	}

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