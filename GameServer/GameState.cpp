#include "pch.h"
#include "GameState.h"

GameState::GameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize) : SessionManager(sessionFactory, maxSessionCount, inMaxBufferSize)
{
	
}

GameState::~GameState()
{
}

GameTaskPtr GameState::GetGameTask()
{
	if (mService == nullptr)
	{
		return nullptr;
	}

	TaskManagerPtr taskManager = mService->GetTaskManager();
	if (taskManager == nullptr)
	{
		return nullptr;
	}

	GameTaskPtr gameTask = std::static_pointer_cast<GameTask>(taskManager);
	if (gameTask == nullptr)
	{
		return nullptr;
	}

	return gameTask;
}

void GameState::SetServerID(const int32 inServerID)
{
	mServerID = inServerID;
}

void GameState::SetServerName(const std::string& inServerName)
{
	mServerName = inServerName;
}

void GameState::SetServerIP(const std::string& inServerIP)
{
	mServerIP = inServerIP;
}

void GameState::SetServerPort(const int32 inPort)
{
	mServerPort = inPort;
}

void GameState::SetServerLevel(const std::string& inServerLevel)
{
	mServerLevel = inServerLevel;
}
