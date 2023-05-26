#include "pch.h"
#include "GameState.h"

GameState::GameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize) : SessionManager(sessionFactory, maxSessionCount, inMaxBufferSize)
{
}

GameState::~GameState()
{
}

bool GameState::PushNetworkTask()
{
	const SessionManagerRef sessionManagerRef = weak_from_this();

	mWorld = std::make_shared<World>(sessionManagerRef, L"World");

	mNetworkTasks.emplace_back(mWorld);
	return true;
}

WorldPtr GameState::GetWorld()
{
	return mWorld;
}
