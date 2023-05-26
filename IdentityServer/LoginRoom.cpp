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

	if (true == FindPlayer(inPlayerState))
	{
		return;
	}

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(inPlayerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return;
	}

	if (false == gameState->CreateRemotePlayer(inPlayerState))
	{
		return;
	}

	const int64 remoteID = inPlayerState->mRemotePlayer->mRemoteID;
	std::pair<int64, PlayerStatePtr> newPlayer = std::make_pair(remoteID, inPlayerState);
	auto ret = mPlayers.insert(newPlayer);
	mPlayersCount++;

	Protocol::S2C_EnterIdentityServer packet;
	packet.set_remote_id(remoteID);
	packet.set_error(ret.second);

	PacketSessionPtr session = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, packet);
	session->Send(sendBuffer);

	GameObjectLog(L"[LoginRoom::Enter] Enter player (CUR : %d)\n", mPlayersCount);
}

void LoginRoom::Leave(PlayerStatePtr inPlayerState)
{
	const int64 remoteID = inPlayerState->mRemotePlayer->mRemoteID;
	size_t isErase = mPlayers.erase(remoteID);

	mPlayersCount--;
	GameObjectLog(L"[LoginRoom::Leave] Leave player (CUR : %d)\n", mPlayersCount);
}

void LoginRoom::Broadcast(PlayerStatePtr inPlayerState)
{
}

void LoginRoom::Signin(PlayerStatePtr inPlayerState, Protocol::C2S_Singin inPacket)
{
	if (false == FindPlayer(inPlayerState))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_Singin_Requset(packetSession, inPacket);

}

void LoginRoom::Signup(PlayerStatePtr inPlayerState, Protocol::C2S_Singup inPacket)
{
	if (false == FindPlayer(inPlayerState))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_Singup_Requset(packetSession, inPacket);
}

void LoginRoom::EmailVerified(PlayerStatePtr inPlayerState, Protocol::C2S_EmailVerified inPacket)
{
	if (false == FindPlayer(inPlayerState))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_EmailVerified_Requset(packetSession, inPacket);
}

void LoginRoom::LoadCharacters(PlayerStatePtr inPlayerState, Protocol::C2S_LoadCharacters inPacket)
{
	if (false == FindPlayer(inPlayerState))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_LoadCharacters_Requset(packetSession, inPacket);
}

void LoginRoom::CreateCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_CreateCharacter inPacket)
{
	if (false == FindPlayer(inPlayerState))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_CreateCharacter_Requset(packetSession, inPacket);
}

void LoginRoom::DeleteCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_DeleteCharacter inPacket)
{
	if (false == FindPlayer(inPlayerState))
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_DeleteCharacter_Requset(packetSession, inPacket);
}

bool LoginRoom::FindPlayer(PlayerStatePtr& inPlayerState)
{
	if (nullptr == inPlayerState->mRemotePlayer)
	{
		return false;
	}

	const int64 remoteID = inPlayerState->mRemotePlayer->mRemoteID;
	auto player = mPlayers.find(remoteID);
	return (player != mPlayers.end());
}
