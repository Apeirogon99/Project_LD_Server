#pragma once

class World : public GameObject
{
public:
	World(const SessionManagerRef& inSessionManager, const WCHAR* inName);
	virtual ~World();

protected:
	virtual void Initialization() override;
	virtual void Destroy() override;

public:
	virtual void Tick() override;

public:
	void Enter(PlayerStatePtr inPlayerState);
	void Leave(PlayerStatePtr inPlayerState);
	void Broadcast(SendBufferPtr sendBuffer);

	void AppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState);
	void DisAppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState);
	void MoveDestination(PlayerStatePtr inPlayerState, Protocol::C2S_MovementCharacter inPakcet);

private:
	std::map<int64, PlayerStatePtr> mPlayers;
	int64 mPlayersCount;
};

