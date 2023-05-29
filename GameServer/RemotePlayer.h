#pragma once
class RemotePlayer : public std::enable_shared_from_this<RemotePlayer>
{
public:
	RemotePlayer() = default;
	RemotePlayer(const PlayerStateRef& inPlayerState, const int64 inRemoteID);
	~RemotePlayer() {};

public:
	bool Init();

public:


	Inventoryptr mInventory;
	int64 mRemoteID;
	PlayerStateRef mPlayerState;

private:
	CharacterPtr mCharacter;
};

