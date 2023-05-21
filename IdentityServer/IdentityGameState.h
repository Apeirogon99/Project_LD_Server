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
	virtual bool PushNetworkTask() override;

public:
	LoginRoomPtr& GetRoom() { return mLoginRoom; }

private:
	LoginRoomPtr	mLoginRoom;
};

