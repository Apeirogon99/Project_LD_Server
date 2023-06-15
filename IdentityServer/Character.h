#pragma once

class Character
{
public:
	Character();
	virtual ~Character();

public:
	void			LoadCharacter(const int32 inID, const Protocol::SCharacterData& inData);

public:
	const int64		GetID()				{ return mID; }
	const int8*		GetName()			{ return mData.name().c_str(); }

public:
	int64						mID;
	Protocol::SCharacterData	mData;
};

