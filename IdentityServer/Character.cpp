#include "pch.h"
#include "Character.h"

Character::Character()
{

}

Character::~Character()
{

}

void Character::LoadCharacter(const int32 inCharacterID, const Protocol::SCharacterData& inCharacterData)
{
	mCharacterID = inCharacterID;
	mCharacterData.CopyFrom(inCharacterData);
}
