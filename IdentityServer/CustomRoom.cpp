#include "pch.h"
#include "CustomRoom.h"

CustomRoom::CustomRoom() : GameObject(L"CustomRoom")
{
}

CustomRoom::~CustomRoom()
{
}

void CustomRoom::OnInitialization()
{
}

void CustomRoom::OnDestroy()
{
}

void CustomRoom::OnTick(const int64 inDeltaTime)
{
}

bool CustomRoom::IsValid()
{
	return (this->GetOwner().lock() != nullptr);
}

void CustomRoom::CreateCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_CreateCharacter inPacket)
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

	characterManager->CreateCharacter(inPacket);
}
