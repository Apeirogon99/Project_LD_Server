#include "pch.h"
#include "IdentityGameState.h"

IdentityGameState::IdentityGameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize) : SessionManager(sessionFactory, maxSessionCount, inMaxBufferSize)
{

}

IdentityGameState::~IdentityGameState()
{
}

bool IdentityGameState::FindWaitingTravelSession(const std::string& inToken, PlayerStatePtr& outPlayerState)
{
	for (auto session : mSessions)
	{

		if (session->GetSessionMode() != ESessionMode::Client)
		{
			continue;
		}

		PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
		if (nullptr == playerState)
		{
			continue;
		}

		LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(playerState->GetRemotePlayer());
		if (nullptr == remotePlayer)
		{
			continue;
		}

		IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
		if (nullptr == identityManager)
		{
			continue;
		}

		const std::string waitToken = identityManager->GetToken();
		const std::string authToken = inToken;
		if (waitToken.compare(authToken) == 0)
		{
			outPlayerState = playerState;
			return true;
		}
	}
	return false;
}

IdentityTaskPtr IdentityGameState::GetTask()
{
	return std::static_pointer_cast<IdentityTask>(GetService()->GetTaskManager());
}
