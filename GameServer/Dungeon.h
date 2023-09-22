#pragma once

enum class EDungeonState
{
	State_Ready,
	State_Play,
	State_Reset,
	State_Stop
};

class Dungeon : public GameWorld
{
public:
	Dungeon();
	virtual ~Dungeon();

	Dungeon(const Dungeon&) = delete;
	Dungeon(Dungeon&&) noexcept = delete;

	Dungeon& operator=(const Dungeon&) = delete;
	Dungeon& operator=(Dungeon&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void SetDungeonID(int32 inDungeonID);
	void CreateDungeon(PlayerStatePtr inPlayerState);

	void InitDungeon();
	void CompleteLoadDungeon(PlayerStatePtr inPlayerState);

protected:
	void CreateStageA();
	void CreateStageB();
	void CreateBossStage();

	bool IsCheckStage(int32 inStageCount);
	bool CheackStageA();
	bool CheackStageB();
	bool CheackBossStage();

	void ClearStageA();
	void ClearStageB();
	void ClearBossStage();

	bool IsEmptyEnemy();

public:
	void PlaySequence(int32 inSequenceNumber);
	void SkipSequence();
	void EndSequence();

public:
	bool IsReady() const;
	bool IsPlay() const;
	bool IsDeathPlayers();

private:
	int32 mDungeonID;
	EDungeonState mState;

	GameWorldRef mOriginWorld;
	int32 mMaxPlayers;
	Location mPlayerStart;

	int32 mStageCount;

	bool mIsPlaySequence;

	std::vector<std::function<bool(Dungeon&)>> mConditionStageFunc;
	std::vector<std::function<void(Dungeon&)>> mCreateStageFunc;
	std::vector<std::function<bool(Dungeon&)>> mCheckStateFunc;
	std::vector<std::function<void(Dungeon&)>> mClearStateFunc;
};

