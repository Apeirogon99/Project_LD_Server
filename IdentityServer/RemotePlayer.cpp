#include "pch.h"
#include "RemotePlayer.h"
#include "Character.h"

RemotePlayer::RemotePlayer() : GameObject(L"RemotePlayer"), mRoomType(ERoomType::None), mToken(""), mGlobalID(-1), mServerID(-1)
{

}

RemotePlayer::~RemotePlayer()
{

}

void RemotePlayer::Initialization() 
{
	mCharacters.reserve(MAX_CHARACTER);
}

void RemotePlayer::Destroy()
{
	for (CharacterPtr character : mCharacters)
	{
		character.reset();
	}
}

void RemotePlayer::Tick()
{

}

bool RemotePlayer::IsValid()
{
	return (mGlobalID != -1) && (mServerID != -1) && (mRoomType != ERoomType::None);
}

void RemotePlayer::SetRoomType(const ERoomType inRoomType)
{
	mRoomType = inRoomType;
}

void RemotePlayer::SetGlobalID(const int32 inGlobalID)
{
	mGlobalID = inGlobalID;
}

void RemotePlayer::SetServerID(const int32 inServerID)
{
	mServerID = inServerID;
}

void RemotePlayer::SetToken(const std::string& inToken)
{
	mToken = inToken;
}

void RemotePlayer::SetCharacters(const std::vector<CharacterPtr>& inCharacters)
{
}

CharacterPtr RemotePlayer::GetCharacter(const int8* inName)
{
	for (CharacterPtr character : mCharacters)
	{
		if (strcmp(character->GetName(), inName) == 0)
		{
			return character;
		}
	}

	return nullptr;
}

CharacterPtr RemotePlayer::GetCharacter(const int32 inSeat)
{
	if (0 <= inSeat && inSeat < MAX_CHARACTER)
	{
		return mCharacters.at(inSeat);
	}

	return nullptr;
}
