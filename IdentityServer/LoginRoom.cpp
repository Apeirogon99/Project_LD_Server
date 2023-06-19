#include "pch.h"
#include "LoginRoom.h"

LoginRoom::LoginRoom(WorldRef inWorld) : GameObject(L"LoginRoom"), mWorld(inWorld)
{
	
}

LoginRoom::~LoginRoom()
{
	
}

void LoginRoom::Initialization()
{
	
}

void LoginRoom::Destroy()
{
	
}

void LoginRoom::Tick()
{
}

bool LoginRoom::IsValid()
{
	return true;
}

void LoginRoom::Signin(PlayerStatePtr inPlayerState, Protocol::C2S_Singin inPacket)
{
	WorldPtr world = mWorld.lock();
	if (nullptr == world)
	{
		return;
	}

	RemotePlayerPtr remotePlayer = inPlayerState->GetRemotePlayer();
	if (false == world->IsValidPlayer(remotePlayer))
	{
		return;
	}

	if (GetRoomType() != remotePlayer->GetRoomType())
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_Singin_Requset(packetSession, inPacket);

}

void LoginRoom::Signup(PlayerStatePtr inPlayerState, Protocol::C2S_Singup inPacket)
{
	WorldPtr world = mWorld.lock();
	if (nullptr == world)
	{
		return;
	}

	RemotePlayerPtr remotePlayer = inPlayerState->GetRemotePlayer();
	if (false == world->IsValidPlayer(remotePlayer))
	{
		return;
	}

	if (GetRoomType() != remotePlayer->GetRoomType())
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_Singup_Requset(packetSession, inPacket);
}

void LoginRoom::EmailVerified(PlayerStatePtr inPlayerState, Protocol::C2S_EmailVerified inPacket)
{
	WorldPtr world = mWorld.lock();
	if (nullptr == world)
	{
		return;
	}

	RemotePlayerPtr remotePlayer = inPlayerState->GetRemotePlayer();
	if (false == world->IsValidPlayer(remotePlayer))
	{
		return;
	}

	if (GetRoomType() != remotePlayer->GetRoomType())
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_EmailVerified_Requset(packetSession, inPacket);
}

void LoginRoom::LoadServerRequest(PlayerStatePtr inPlayerState, Protocol::C2S_LoadServer inPacket)
{
	WorldPtr world = mWorld.lock();
	if (nullptr == world)
	{
		return;
	}

	RemotePlayerPtr remotePlayer = inPlayerState->GetRemotePlayer();
	if (false == world->IsValidPlayer(remotePlayer))
	{
		return;
	}

	if (GetRoomType() != remotePlayer->GetRoomType())
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_Select_Character_Request(packetSession, remotePlayer->GetGlobalID());
}

void LoginRoom::SelectServer(PlayerStatePtr inPlayerState, Protocol::C2S_SelectServer inPacket)
{
	WorldPtr world = mWorld.lock();
	if (nullptr == world)
	{
		return;
	}

	RemotePlayerPtr remotePlayer = inPlayerState->GetRemotePlayer();
	if (false == world->IsValidPlayer(remotePlayer))
	{
		return;
	}

	if (GetRoomType() != remotePlayer->GetRoomType())
	{
		return;
	}

	const int32 serverID = inPacket.server_id();
	remotePlayer->SetServerID(serverID);

	remotePlayer->SetRoomType(ERoomType::SelectRoom);

	Protocol::S2C_SelectServer selectServerPacket;
	selectServerPacket.set_error(true);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(packetSession, selectServerPacket);
	packetSession->Send(sendBuffer);
}
