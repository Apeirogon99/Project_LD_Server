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
	WorldPtr world = std::make_shared<World>(L"World", GetTaskPtr());
	GameObjectPtr worldGameObject = world->GetGameObjectPtr();
	PushTask(worldGameObject);
}

GameDatasPtr GameTask::GetGameDatas()
{
	return std::static_pointer_cast<GameDatas>(GetService()->GetDataManager());
}

GameTaskPtr GameTask::GetTaskPtr()
{
	return std::static_pointer_cast<GameTask>(shared_from_this());
}

WorldPtr GameTask::GetWorld()
{
	GameObjectPtr object;
	bool findObject = FindTask(L"World", object);
	if (false == findObject)
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
