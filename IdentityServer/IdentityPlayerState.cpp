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

void IdentityPlayerState::OnDisconnected()
{
	PlayerStateLog(L"[IdentityPlayerState::OnDisconnected] Dissconnect player");

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

void IdentityPlayerState::PlayerStateLog(const WCHAR* log, ...)
{
	std::wstring message;
	const int64 remoteID = mRemotePlayer->mRemoteID;
	message.append(L"[ID : ");
	message.append(std::to_wstring(remoteID));
	message.append(L"]");
	message.append(log);
	this->SessionLog(message.c_str());
}
