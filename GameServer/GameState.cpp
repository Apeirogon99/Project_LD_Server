#include "pch.h"
#include "GameState.h"

GameState::GameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize) : SessionManager(sessionFactory, maxSessionCount, inMaxBufferSize)
{
	mWorld = std::make_shared<World>(weak_from_this(), L"World");
}

GameState::~GameState()
{
}

bool GameState::ProcessNetworkTask(const int64 inServiceTimeStamp)
{
	mWorld->Execute(inServiceTimeStamp);
	return true;
}

WorldPtr GameState::GetWorld()
{
	return mWorld;
}
