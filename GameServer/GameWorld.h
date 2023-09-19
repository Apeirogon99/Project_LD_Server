#pragma once

enum class EActorType
{
	Unspecified,
	Obstruction,
	Visible,
	Player,
	PickUp,
	Enemy,
	EnemyAttack,
};

class WorldPlayerInfo
{
public:
	WorldPlayerInfo() : mCharacterID(0), mCharacterName("") {}
	~WorldPlayerInfo() {}

public:
	void SetRemoteID(const int64& inRemoteID) { mCharacterID = inRemoteID; }
	void SetCharacterName(const std::string& inCharacterName) { mCharacterName = inCharacterName; }

public:
	const int64& GetRemoteID() const { return mCharacterID; }
	const std::string& GetCharacterName() const { return mCharacterName; }

private:
	int64		mCharacterID;
	std::string mCharacterName;
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
	void LevelTravel(GameWorldPtr inTravelWorld, int64 inLeaveRemoteID);

	void WorldChat(PlayerStatePtr inPlayerState, const int64 inWorldTime, std::string inMessage);

	void VisibleAreaInit(PlayerStatePtr inPlayerState);
	void VisibleAreaSync(const int64 inDeltaTime);
	void CheackToken();

	void PushCharacterIDandRemoteID(const int64& inCharacterID, const std::string& inCharacterName, const int64& inRemoteID);
	void ReleaseCharacterIDandRemoteID(const int64& inCharacterID);
	bool IsValidCharacter(const int64& inCharacterID, GameRemotePlayerPtr& outRemotePlayerPtr);
	bool IsValidCharacter(const std::string& inCharacterName, GameRemotePlayerPtr& outRemotePlayerPtr);
	bool IsValidCharacter(const int64& inCharacterID);

	bool IsValidPlayer(const int64& inRemoteID, GameRemotePlayerPtr& outRemotePlayerPtr);

public:
	void RefreshWorldObserver();
	void MakeWorldObstruction(const EGameDataType inGameData);
	void MakeWorldObstruction(const EGameDataType inGameData, const int32 inIndex);

protected:
	void DoMakeWorldObstruction(const CSVRow& inRow);

public:
	const EnemySpawnerManagerPtr&	GetEnemySpawnerManager();
	const DungeonManagerPtr&		GetDungeonManager();

private:
	std::vector<class Token>			mTokens;
	std::map<int64, WorldPlayerInfo>	mPlayerIDs;

	EnemySpawnerManagerPtr				mSpawnerManager;
	DungeonManagerPtr					mDungeonManager;
};