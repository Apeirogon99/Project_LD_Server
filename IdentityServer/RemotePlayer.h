#pragma once
class RemotePlayer
{
public:
	RemotePlayer() : mToken(""), mRemoteID(-1), mGlobalID(-1) {}
	virtual ~RemotePlayer() {}

public:
	std::string	mToken;
	int64		mRemoteID;
	int32		mGlobalID;
};

