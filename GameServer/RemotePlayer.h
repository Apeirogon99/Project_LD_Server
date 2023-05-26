#pragma once
class RemotePlayer
{
public:
	RemotePlayer() = default;
	RemotePlayer(const int64 inRemoteID);
	~RemotePlayer() {};

public:
	Protocol::SCharacterData mCharacterData;
	int64 mID;
};

