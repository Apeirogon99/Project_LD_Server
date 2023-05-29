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
	PlayerStateRef	GetPlayerState()	const { return mPlayerState; }
	const int64		GetRemoteID()		const { return mRemoteID; }
	Inventoryptr	GetInventory()		const { return mInventory; }
	CharacterPtr	GetCharacter()		const { return mCharacter; }


private:
	PlayerStateRef	mPlayerState;
	int64			mRemoteID;
	Inventoryptr	mInventory;
	CharacterPtr	mCharacter;
};

