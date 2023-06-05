#include "pch.h"
#include "GameTask.h"

GameTask::GameTask() : TaskManager()
{
	mWorld = std::make_shared<World>(L"World");
	GameObjectPtr gamePtr = mWorld->GetGameObjectPtr();
	PushTask(gamePtr);
}

GameTask::~GameTask()
{
	
}

WorldPtr GameTask::GetWorld()
{
	GameObjectPtr object = FindTask(L"World");
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
