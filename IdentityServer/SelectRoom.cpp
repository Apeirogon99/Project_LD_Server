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

	const int32 seat = inPacket.seat();
	CharacterPtr character = remotePlayer->GetCharacter(seat);
	if (nullptr == character)
	{
		return;
	}

	const int characterID = character->GetCharacterID();

	Protocol::S2C_StartGame startGamePacket;
	startGamePacket.set_server_id(1);
	startGamePacket.set_server_name("L_NecromancerDungeon");
	startGamePacket.set_ip("116.41.116.247");
	startGamePacket.set_port(10000);
	startGamePacket.set_error(0);

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
