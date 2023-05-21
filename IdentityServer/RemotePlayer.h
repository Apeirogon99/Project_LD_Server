#pragma once
class RemotePlayer
{
public:
	RemotePlayer();
	virtual ~RemotePlayer();

public:
	Character* FindCharacter(const int8* name);

public:
	ERoomType	mRoomType;

	std::string	mToken;
	int64		mRemoteID;
	int32		mGlobalID;

	Character	mCharacters[4];
};

