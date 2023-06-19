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
	if (GetSessionMode() == ESessionMode::Server)
	{
		WinSocketPtr gameServerSocket = GetWinSocket();
		IPAddressPtr gameServerIPAddr = std::make_shared<IPAddress>();
		gameServerIPAddr->SetIp(L"116.41.116.247", 10000);
		gameServerSocket->Connect(gameServerIPAddr);

		RegisterRecv();
	}
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

	WorldPtr world = task->GetWorld();
	if (world == nullptr)
	{
		return;
	}

	const int64 Priority = gameState->GetServiceTimeStamp();
	bool ret = world->PushTask(Priority, &World::Leave, this->GetPlayerStatePtr());
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

void IdentityPlayerState::SetRemotePlayer(RemotePlayerPtr& inRemotePlayer)
{
	mRemotePlayer = inRemotePlayer;
}
