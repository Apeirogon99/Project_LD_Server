#pragma once
class IdentityGameState : public SessionManager
{
public:
	IdentityGameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize);
	virtual ~IdentityGameState();

	IdentityGameState(const IdentityGameState& player) = delete;
	IdentityGameState(IdentityGameState&& player) = delete;
	IdentityGameState& operator=(const IdentityGameState& player) = delete;
	IdentityGameState& operator=(IdentityGameState&& player) = delete;

public:
	bool FindWaitingTravelSession(const std::string& token, PlayerStatePtr& outPlayerState);

public:
	IdentityTaskPtr GetTask();
};

