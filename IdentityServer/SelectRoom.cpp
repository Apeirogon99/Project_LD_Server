#include "pch.h"
#include "SelectRoom.h"

SelectRoom::SelectRoom() : GameObject(L"SelectRoom")
{

}

SelectRoom::~SelectRoom()
{
}

void SelectRoom::OnInitialization()
{
}

void SelectRoom::OnDestroy()
{
}

void SelectRoom::OnTick(const int64 inDeltaTime)
{
}

bool SelectRoom::IsValid()
{
	return (this->GetOwner().lock() != nullptr);
}

void SelectRoom::LoadCharacters(PlayerStatePtr inPlayerState, Protocol::C2S_LoadCharacters inPacket)
{
	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	CharacterManagerPtr characterManager = remotePlayer->GetCharacterManager();
	if (nullptr == characterManager)
	{
		return;
	}

	characterManager->LoadCharacter();
}

void SelectRoom::DeleteCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_DeleteCharacter inPacket)
{
	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	CharacterManagerPtr characterManager = remotePlayer->GetCharacterManager();
	if (nullptr == characterManager)
	{
		return;
	}

	characterManager->DeleteCharacter(inPacket);
}


void SelectRoom::StartCharacterRequest(PlayerStatePtr inPlayerState, Protocol::C2S_StartGame inPacket)
{
	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return;
	}

	CharacterManagerPtr characterManager = remotePlayer->GetCharacterManager();
	if (nullptr == characterManager)
	{
		return;
	}

	const int32& characterID = inPacket.character_id();
	LoginCharacterPtr character = characterManager->GetLoginCharacter(characterID);
	if (nullptr == character)
	{
		return;
	}

	SessionPtr gameServerSession;
	inPlayerState->GetSessionManager()->FindServerSession(gameServerSession);

	Protocol::C2S_TravelServer travelServerPacket;
	travelServerPacket.set_token(identityManager->GetToken());
	travelServerPacket.set_global_id(identityManager->GetGlobalID());
	travelServerPacket.set_character_id(character->GetCharacterID());

	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(nullptr, travelServerPacket);
	gameServerSession->Send(sendBuffer);
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