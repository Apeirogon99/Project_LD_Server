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
	PushTask(std::make_shared<World>(L"World", shared_from_this())->GetGameObjectPtr());
}

WorldPtr GameTask::GetWorld()
{
	GameObjectPtr object = FindTask(0);
	if (nullptr == object)
	{
		return nullptr;
	}
	
	WorldPtr world = std::static_pointer_cast<World>(object);
	if (nullptr == world)
	{
		return nullptr;
	}

	return world;
}
