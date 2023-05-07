#include "pch.h"
#include "IdentityGameState.h"

IdentityGameState::IdentityGameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize) : SessionManager(sessionFactory, maxSessionCount, inMaxBufferSize)
{
	mLoginRoom = std::make_shared<LoginRoom>();
}

IdentityGameState::~IdentityGameState()
{
}

bool IdentityGameState::InitNetworkTask()
{
	mNetworkTasks.emplace_back(mLoginRoom->shared_from_this());

	return true;
}
