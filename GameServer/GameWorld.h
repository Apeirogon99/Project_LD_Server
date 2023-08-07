#pragma once

enum class EActorType
{
	Unspecified,
	Visible,
	Player,
	PickUp,
	Enemy,
};

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

	void WorldChat(PlayerStatePtr inPlayerState, const int64 inWorldTime, std::string inMessage);

	void VisibleAreaInit(PlayerStatePtr inPlayerState);
	void VisibleAreaSync(const int64 inDeltaTime);
	void CheackToken();

	void PushCharacterIDandRemoteID(const int64& inCharacterID, const int64& inPlayerID);
	void ReleaseCharacterIDandRemoteID(const int64& inCharacterID);
	bool IsValidPlayer(const int64& inCharacterID, GameRemotePlayerPtr& outRemoteClientPtr);
	bool IsValidPlayer(const int64& inCharacterID);

protected:
	void RefreshWorldObserver();

public:
	const EnemySpawnerManagerPtr& GetEnemySpawnerManager();

private:
	std::vector<class Token>			mTokens;
	std::map<int64, int64>				mPlayerIDs;
	EnemySpawnerManagerPtr				mSpawnerManager;
};