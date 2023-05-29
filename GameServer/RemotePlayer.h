#pragma once
class RemotePlayer : std::enable_shared_from_this<RemotePlayer>
{
public:
	RemotePlayer() = default;
	RemotePlayer(const PlayerStateRef& inPlayerState, const int64 inRemoteID);
	~RemotePlayer() {};

public:
	int32 mCharacterID;
	Protocol::SCharacterData mCharacterData;
	Inventory mInventory;
	int64 mRemoteID;
	PlayerStateRef mPlayerState;
};

