#include "pch.h"
#include "RemotePlayer.h"

RemotePlayer::RemotePlayer(const PlayerStateRef& inPlayerState, const int64 inRemoteID) : mPlayerState(inPlayerState), mRemoteID(inRemoteID), mInventory(weak_from_this(), 6, 5)
{

}
