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
	LoginWorldPtr world = std::make_shared<LoginWorld>();
	PushTask(world->GetGameObjectPtr());
}

IdentityTaskPtr IdentityTask::GetTaskPtr()
{
	return std::static_pointer_cast<IdentityTask>(shared_from_this());
}

LoginWorldPtr IdentityTask::GetWorld()
{
	GameObjectPtr object;
	FindTask(L"LoginWorld", object);
	if (nullptr == object)
	{
		return nullptr;
	}

	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(object);
	if (nullptr == world)
	{
		return nullptr;
	}

	return world;
}
