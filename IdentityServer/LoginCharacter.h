#pragma once

class LoginCharacter
{
public:
	LoginCharacter();
	virtual ~LoginCharacter();

public:
	void			LoadCharacter(const int32 inCharacterID, const Protocol::SCharacterData* inData);

public:
	const int32		GetCharacterID()	{ return mCharacterID; }
	const int8*		GetName()			{ return mCharacterData.name().c_str(); }

public:
	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;
};

