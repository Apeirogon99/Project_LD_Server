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

void SelectRoom::Tick(const int64 inDeltaTime)
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

void SelectRoom::StartCharacterRequest(PlayerStatePtr inPlayerState, Protocol::C2S_StartGame inPacket)
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

	SessionPtr gameServerSession;
	inPlayerState->GetSessionManager()->FindServerSession(gameServerSession);

	Protocol::C2S_TravelServer travelServerPacket;
	travelServerPacket.set_token(remotePlayer->GetToken());
	travelServerPacket.set_global_id(remotePlayer->GetGlobalID());
	travelServerPacket.set_character_id(character->GetCharacterID());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(gameServerSession);
	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(packetSession, travelServerPacket);
	packetSession->Send(sendBuffer);
}

void SelectRoom::StartCharacterRespone(PlayerStatePtr inPlayerState, Protocol::S2C_TravelServer inPacket)
{
	
	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(inPlayerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return;
	}

	
	PlayerStatePtr waitingTravelSession;
	bool isValid = gameState->FindWaitingTravelSession(inPacket.token(), waitingTravelSession);
	if (false == isValid)
	{
		return;
	}

	Protocol::S2C_StartGame startGamePacket;
	const int32 travelError = inPacket.error();
	if (travelError != ErrorToInt(EGameErrorType::INVALID_TOKEN))
	{
		startGamePacket.set_server_id(inPacket.server_id());
		startGamePacket.set_travel_level(inPacket.travel_level());
		startGamePacket.set_ip(inPacket.ip());
		startGamePacket.set_port(inPacket.port());
		startGamePacket.set_error(travelError);
	}
	startGamePacket.set_error(travelError);
	
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(waitingTravelSession);
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
