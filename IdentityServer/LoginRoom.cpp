#include "pch.h"
#include "LoginRoom.h"

LoginRoom::LoginRoom(const SessionManagerRef& inSessionManager, const WCHAR* inLoginRoomName) : GameObject(inSessionManager, inLoginRoomName)
{
	
}

LoginRoom::~LoginRoom()
{
	
}

void LoginRoom::Initialization()
{
	mPlayersCount = 0;
}

void LoginRoom::Destroy()
{
	mPlayers.clear();
}

void LoginRoom::Tick()
{
}

void LoginRoom::Enter(PlayerStatePtr inPlayerState)
{
	const int64 remoteID = inPlayerState->mRemotePlayer->mRemoteID;
	std::pair<int64, PlayerStatePtr> newPlayer = std::make_pair(remoteID, inPlayerState);
	auto ret = mPlayers.insert(newPlayer);

	Protocol::S2C_EnterIdentityServer packet;
	packet.set_remote_id(remoteID);
	packet.set_error(ret.second);

	PacketSessionPtr session = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, packet);
	session->Send(sendBuffer);

	mPlayersCount++;
	GameObjectLog(L"[LoginRoom::Enter] Enter player (CUR : %d)\n", mPlayersCount);
}

void LoginRoom::Leave(PlayerStatePtr inPlayerState)
{
	const int64 remoteID = inPlayerState->mRemotePlayer->mRemoteID;
	size_t isErase = mPlayers.erase(remoteID);

	mPlayersCount--;
	GameObjectLog(L"[LoginRoom::Leave] Leave player (CUR : %d)\n", mPlayersCount);
}

void LoginRoom::Deliver(PlayerStatePtr inPlayerState)
{
	PacketSessionPtr session = std::static_pointer_cast<PacketSession>(inPlayerState);

	Protocol::S2C_Test packet;
	packet.set_value(1);
	packet.set_s_value("Hello");
	packet.set_time_stamp(1000);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, packet);
	session->Send(sendBuffer);
}

void LoginRoom::Broadcast(PlayerStatePtr inPlayerState)
{
}