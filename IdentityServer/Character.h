#include "Character.h"
#pragma once
class Character
{
public:
	Character() : mSeat(-1), mCharacterID(-1)
	{
		memset(mName, NULL, 20);
	}
	Character(const int32 inSeat) : mSeat(inSeat), mCharacterID(-1)
	{
		memset(mName, NULL, 20);
	}

	Character(const Character& inCharacter)
	{
		mSeat			= inCharacter.mSeat;
		mCharacterID	= inCharacter.mCharacterID;
		strcpy_s(mName, 20, inCharacter.mName);
	}

	Character& operator=(const Character& inCharacter)
	{
		mSeat			= inCharacter.mSeat;
		mCharacterID	= inCharacter.mCharacterID;
		strcpy_s(mName, 20, inCharacter.mName);
		return *this;
	}

	~Character()
	{

	}

public:
	bool IsValid()
	{
		return (mCharacterID == -1 ? false : true);
	}

	void SetCharacter(const int64 inCharacterID, const int8* inName)
	{
		mCharacterID = inCharacterID;
		strcpy_s(mName, 20, inName);
		mName[strlen(inName) + 1] = '\n';
	}

public:
	int32		mSeat;
	int64		mCharacterID;
	int8		mName[20];
};

