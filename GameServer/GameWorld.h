#pragma once

class GameWorld : public World
{
public:
	GameWorld(const WCHAR* inName);
	virtual ~GameWorld();

protected:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void ServerTravel(PlayerStatePtr inPlayerState, Protocol::C2S_TravelServer inPacket);
	void Enter(PlayerStatePtr inPlayerState, Protocol::C2S_EnterGameServer inPacket);
	void Leave(PlayerStatePtr inPlayerState);

	void VisibleAreaInit(PlayerStatePtr inPlayerState);
	void VisibleAreaSync();
	void CheackToken();

private:
	std::vector<class Token>			mTokens;
};