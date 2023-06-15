#include "pch.h"
#include "CustomRoom.h"

CustomRoom::CustomRoom(WorldRef inWorld) : GameObject(L"CustomRoom"), mWorld(inWorld)
{
}

CustomRoom::~CustomRoom()
{
}

void CustomRoom::Initialization()
{
}

void CustomRoom::Destroy()
{
}

void CustomRoom::Tick()
{
}

bool CustomRoom::IsValid()
{
	return false;
}

void CustomRoom::CreateCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_CreateCharacter inPacket)
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

	//if(GetRoomType() != remotePlayer->GetRoomType())
	//{
	//	return;
	//}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_CreateCharacter_Requset(packetSession, inPacket);
}
