#include "pch.h"
#include "IdentityGameState.h"

IdentityGameState::IdentityGameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize) : SessionManager(sessionFactory, maxSessionCount, inMaxBufferSize)
{

}

IdentityGameState::~IdentityGameState()
{
}

bool IdentityGameState::PushNetworkTask()
{
	const SessionManagerRef sessionManagerRef = weak_from_this();
	mLoginRoom = std::make_shared<LoginRoom>(sessionManagerRef, L"LoginRoom_1");
	mNetworkTasks.emplace_back(mLoginRoom);
	return true;
}

bool IdentityGameState::CreateRemotePlayer(PlayerStatePtr& inPlayerState)
{
	//TODO
	static int64 remoteID = 1;

	RemotePlayerPtr& remotePlayer = inPlayerState.get()->mRemotePlayer;
	remotePlayer = std::make_shared<RemotePlayer>();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	remotePlayer->mRemoteID = remoteID++;

	return true;
}
