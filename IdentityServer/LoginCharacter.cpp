#include "pch.h"
#include "LoginCharacter.h"

LoginCharacter::LoginCharacter()
{

}

LoginCharacter::~LoginCharacter()
{

}

void LoginCharacter::LoadCharacter(const int32 inCharacterID, const Protocol::SCharacterData* inCharacterData)
{
	mCharacterID = inCharacterID;
	mCharacterData.CopyFrom(*inCharacterData);
}
