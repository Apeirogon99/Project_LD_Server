#pragma once

class LoginCharacter
{
public:
	LoginCharacter();
	virtual ~LoginCharacter();

public:
	void			LoadCharacter(const int32 inCharacterID, const Protocol::SCharacterData* inData);

public:
	const int32			GetCharacterID()	{ return mCharacterID; }
	const std::string&	GetName()			{ return mCharacterData.name(); }

public:
	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;
};

