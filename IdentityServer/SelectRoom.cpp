#include "pch.h"
#include "SelectRoom.h"

SelectRoom::SelectRoom(WorldRef inWorld) : GameObject(L"SelectRoom"), mWorld(inWorld)
{

}

SelectRoom::~SelectRoom()
{
}

void SelectRoom::Initialization()
{
}

void SelectRoom::Destroy()
{
}

void SelectRoom::Tick()
{
}

bool SelectRoom::IsValid()
{
	return false;
}

void SelectRoom::LoadCharacters(PlayerStatePtr inPlayerState, Protocol::C2S_LoadCharacters inPacket)
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
	Handle_LoadCharacters_Requset(packetSession, inPacket);
}

void SelectRoom::StartCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_StartGame inPacket)
{
	Protocol::S2C_StartGame startGamePacket;
	//TODO: 서버에 연결된거 가져와서 보내야함
	startGamePacket.set_ip("192.168.123.112");
	startGamePacket.set_port(10000);
	startGamePacket.set_error(true);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(packetSession, startGamePacket);
	packetSession->Send(sendBuffer);
}

void SelectRoom::DeleteCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_DeleteCharacter inPacket)
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
	Handle_DeleteCharacter_Requset(packetSession, inPacket);
}
