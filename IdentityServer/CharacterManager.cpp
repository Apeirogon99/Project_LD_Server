#include "pch.h"
#include "CharacterManager.h"

CharacterManager::CharacterManager() : GameObject(L"CharacterManager")
{
}

CharacterManager::~CharacterManager()
{
}

void CharacterManager::OnInitialization()
{
}

void CharacterManager::OnDestroy()
{
	this->ClearCharacter();
}

void CharacterManager::OnTick(const int64 inDeltaTime)
{
}

bool CharacterManager::IsValid()
{
	return (this->GetOwner().lock() != nullptr);
}

void CharacterManager::LoadCharacter()
{
	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);

	const int32& globalID = identityManager->GetGlobalID();
	const int32& serverID = identityManager->GetServerID();

	Handle_LoadCharacters_Requset(packetSession, globalID, serverID);
}

void CharacterManager::CreateCharacter(Protocol::C2S_CreateCharacter inPacket)
{
	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);

	const int32& globalID = identityManager->GetGlobalID();
	const int32& serverID = identityManager->GetServerID();

	const Protocol::SCharacterData&			characterData		= inPacket.character_data();
	const Protocol::SCharacterAppearance&	characterAppearance = characterData.appearance();
	const Protocol::SCharacterEqipment&		characterEqipment	= characterData.eqipment();

	Handle_CreateCharacter_Requset(packetSession, characterData.name(), characterData.character_class(), characterAppearance.race(), characterAppearance.seat(), globalID, serverID, characterAppearance.skin_color(), characterEqipment.hair(), characterAppearance.hair_color(), characterAppearance.eye_color(), characterAppearance.eyebrow_color());
}

void CharacterManager::DeleteCharacter(Protocol::C2S_DeleteCharacter inPacket)
{
	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(this->GetOwner().lock());
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
	LoginCharacterPtr character = characterManager->GetLoginCharacter(inPacket.name());

	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);

	const int32& globalID = identityManager->GetGlobalID();
	const int32& serverID = identityManager->GetServerID();
	const int32& characterID	= character->GetCharacterID();

	Handle_DeleteCharacter_Requset(packetSession, globalID, serverID, characterID);
}

void CharacterManager::PushCharacter(const int32 inCharacterID, const Protocol::SCharacterData* inCharacterData)
{
	LoginCharacterPtr newCharacter = std::make_shared<LoginCharacter>();
	newCharacter->LoadCharacter(inCharacterID, inCharacterData);

	std::pair<int32, LoginCharacterPtr> character = std::make_pair(inCharacterID, newCharacter);
	mCharacters.insert(character);
}

void CharacterManager::ReleaseCharacter(const int32 inCharacterID)
{
	auto findCharacter = mCharacters.find(inCharacterID);
	if (findCharacter == mCharacters.end())
	{
		return;
	}

	findCharacter->second.reset();
	mCharacters.erase(inCharacterID);
}

void CharacterManager::ClearCharacter()
{
	for (auto character : mCharacters)
	{
		character.second.reset();
	}
	mCharacters.clear();
}

LoginCharacterPtr CharacterManager::GetLoginCharacter(const int32 inCharacterID)
{
	auto findCharacter = mCharacters.find(inCharacterID);
	if (findCharacter == mCharacters.end())
	{
		return nullptr;
	}

	return findCharacter->second;
}

LoginCharacterPtr CharacterManager::GetLoginCharacter(const std::string& inCharacterName)
{
	for (auto character : mCharacters)
	{
		if (character.second->GetName().compare(inCharacterName) == 0)
		{
			return character.second;
		}
	}

	return nullptr;
}
