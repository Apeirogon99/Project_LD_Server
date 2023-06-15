#include "pch.h"
#include "Character.h"

Character::Character()
{

}

Character::~Character()
{

}

void Character::LoadCharacter(const int32 inID, const Protocol::SCharacterData& inData)
{
	mID = inID;
	mData.CopyFrom(inData);
}
