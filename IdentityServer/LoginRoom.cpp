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

void LoginRoom::LoadServer(PlayerStatePtr inPlayerState, Protocol::C2S_LoadServer inPacket)
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

	std::vector<Protocol::SServerInfo> tempServerInfos;
	Protocol::SServerInfo tempInfo;
	tempInfo.set_id(1);
	tempInfo.set_name("Ÿ��");
	tempInfo.set_state("��Ȱ");
	tempInfo.set_count(2);

	tempServerInfos.push_back(tempInfo);

	Protocol::S2C_LoadServer loadServerPacket;
	for (Protocol::SServerInfo& info : tempServerInfos)
	{
		Protocol::SServerInfo* newInfo = loadServerPacket.add_info();
		newInfo->CopyFrom(info);
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(packetSession, loadServerPacket);
	packetSession->Send(sendBuffer);
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
