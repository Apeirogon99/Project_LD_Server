#include "pch.h"
#include "GameTask.h"

GameTask::GameTask() : TaskManager()
{
	
}

GameTask::~GameTask()
{
	
}

void GameTask::Init()
{
	GameWorldPtr world = std::make_shared<GameWorld>(L"World");
	GameObjectPtr worldGameObject = world->GetGameObjectPtr();
	PushTask(worldGameObject);
}

GameWorldPtr GameTask::GetWorld()
{
	GameObjectPtr object;
	FindTask(1, object);

	GameWorldPtr gameWorld = std::static_pointer_cast<GameWorld>(object);
	if (nullptr == gameWorld)
	{
		return nullptr;
	}

	return gameWorld;
}

GameTaskPtr GameTask::GetTaskPtr()
{
	return std::static_pointer_cast<GameTask>(shared_from_this());
}