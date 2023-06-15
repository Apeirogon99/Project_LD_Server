#include "pch.h"
#include "IdentityTask.h"

IdentityTask::IdentityTask() : TaskManager()
{
}

IdentityTask::~IdentityTask()
{
}

void IdentityTask::Init()
{
	PushTask(std::make_shared<World>(GetTaskPtr())->GetGameObjectPtr());
}

IdentityTaskPtr IdentityTask::GetTaskPtr()
{
	return std::static_pointer_cast<IdentityTask>(shared_from_this());
}

WorldPtr IdentityTask::GetWorld()
{
	GameObjectPtr object;
	FindTask(L"World", object);
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
