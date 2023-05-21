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
	return true;
}
