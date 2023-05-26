#pragma once
class RemotePlayer
{
public:
	RemotePlayer();
	virtual ~RemotePlayer();

public:
	void		Init() {}
	Character*	FindCharacter(const int8* name);

public:
	ERoomType	mRoomType;

	std::string	mToken;
	int64		mRemoteID;
	int32		mGlobalID;
	int32		mServerID;

//private:
	Character	mCharacters[4];
};

