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
	Protocol::S2C_EnterServer enterServer;
	PacketSessionPtr session = GetPacketSessionRef();

	bool valid = session->IsValid();
	if (false == valid)
	{
		enterServer.set_error(-1);
	}
	else
	{
		
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, enterServer);
	Send(sendBuffer);
}

void IdentityPlayerState::OnSend(uint32 len)
{

}

void IdentityPlayerState::OnDisconnected()
{
	Protocol::S2C_LeaveServer leavePacket;
	PacketSessionPtr session = GetPacketSessionRef();
	bool valid = session->IsValid();
	if (false == valid)
	{
		leavePacket.set_error(-1);
	}
	else
	{
		leavePacket.set_error(0);
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, leavePacket);
	Send(sendBuffer);
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