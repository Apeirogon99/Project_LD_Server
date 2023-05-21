#pragma once

class LoginRoom : public GameObject
{
public:
	LoginRoom(const SessionManagerRef& inSessionManager, const WCHAR* inLoginRoomName);
	virtual ~LoginRoom();

protected:
	virtual void Initialization() override;
	virtual void Destroy() override;
	virtual void Tick() override;

public:
	void Enter(PlayerStatePtr inPlayerState);
	void Leave(PlayerStatePtr inPlayerState);
	void Deliver(PlayerStatePtr inPlayerState);
	void Broadcast(PlayerStatePtr inPlayerState);

private:
	int32 mPlayersCount;
	std::map<uint64, PlayerStatePtr> mPlayers;
};

