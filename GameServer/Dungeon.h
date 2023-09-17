#pragma once

enum class EDungeonState
{
	State_Ready,
	State_Play,
	State_Reset,
	State_Stop
};

class Dungeon : public GameObject
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
	void BroadCastDungeon(SendBufferPtr inSendBuffer);
	void ResetDungeon();

public:
	bool IsCreateStage(int32 inStageCount);
	bool ConditionStageA();
	bool ConditionStageB();
	bool ConditionBossStage();

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

private:
	EDungeonState mState;

	bool mIsCreateStage;
	int32 mStageCount;

	std::vector<std::function<bool(Dungeon&)>> mConditionStageFunc;
	std::vector<std::function<void(Dungeon&)>> mCreateStageFunc;
	std::vector<std::function<bool(Dungeon&)>> mCheckStateFunc;
	std::vector<std::function<void(Dungeon&)>> mClearStateFunc;

	std::vector<ActorPtr> mEnemys;
	std::vector<GameRemotePlayerPtr> mPlayers;
};

