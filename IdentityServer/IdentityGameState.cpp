#include "pch.h"
#include "IdentityGameState.h"

IdentityGameState::IdentityGameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize) : SessionManager(sessionFactory, maxSessionCount, inMaxBufferSize)
{

}

IdentityGameState::~IdentityGameState()
{
}

IdentityTaskPtr IdentityGameState::GetTask()
{
	return std::static_pointer_cast<IdentityTask>(GetService()->GetTaskManager());
}
