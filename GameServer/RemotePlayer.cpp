#include "pch.h"
#include "RemotePlayer.h"

RemotePlayer::RemotePlayer(const PlayerStateRef& inPlayerState, const int64 inRemoteID) : mPlayerState(inPlayerState), mRemoteID(inRemoteID)
{

}

bool RemotePlayer::Init()
{
	RemotePlayerRef remotePlayerRef = weak_from_this();
	mInventory = std::make_shared<Inventory>(remotePlayerRef, 6, 15);
	return true;
}
