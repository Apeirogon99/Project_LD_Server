#pragma once

enum class EnemyID
{
	Enemy_Slime = 1,
	Enemy_Nomal_Skeleton,
	Enemy_Warrior_Skeleton,
	Enemy_Archer_Skeleton,
	Enemy_Dark_Skeleton,
	Enemy_Dark_Knight,
	Enemy_Lich_Phase1,
	Enemy_Lich_Phase2,
	Enemy_Lich_Phase3,
	Enemy_Lich_Life_Vessle,
};

class EnemySpawner : public GameObject
{
public:
	EnemySpawner();
	~EnemySpawner();

	EnemySpawner(const EnemySpawner&) = delete;
	EnemySpawner(EnemySpawner&&) noexcept = delete;

	EnemySpawner& operator=(const EnemySpawner&) = delete;
	EnemySpawner& operator=(EnemySpawner&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void SetEnemySpawner(const GameObjectRef& inOwner, const FVector& inLocation, const int32& inEnemyID, const float& inSpawnRange, const int32& inSpawnCount, const int32& inSpawnLoop, const bool& inIsAggresive, const bool& inIsReward, const float& inMaxChaseRange, const float& inSearchRange);
	void SpawnEnemy();

	void NotifyDestroyEnemy(const int64& inGameObjectID);

	void ClearEnemy();

public:
	bool IsRespawn();
	const int32 GetMaxEnemyCount()		const { return mMaxEnmeyCount; }
	const int32 GetCurrentEnemyCount()	const { return mCurEnemyCount; }
	const int32 GetEnemyID()			const { return mEnemyID; }
	const float GetSpawnRange()			const { return mSpawnRange; }
	const FVector GetLocation()			const { return mLocation; }
	const int32 GetSpawnLoop()			const { return mSpawnLoop; }

protected:
	ActorPtr SpawnTemplate();

private:
	GameObjectRef mOwner;

	bool  mIsLoad;
	int64 mRespawnTime;

	int32 mMaxEnmeyCount;
	int32 mCurEnemyCount;

	int32 mEnemyID;
	Stats mEnemyStats;
	float mSpawnRange;
	float mMaxChaseRange;
	float mMaxSearchRange;
	int32 mSpawnLoop;
	bool mIsAggresive;
	bool mIsReward;

	FVector mLocation;

	std::vector<EnemyCharacterPtr> mEnemyCharacters;
};

class EnemySpawnerManager : public GameObject
{
public:
	EnemySpawnerManager();
	~EnemySpawnerManager();

	EnemySpawnerManager(const EnemySpawnerManager&) = delete;
	EnemySpawnerManager(EnemySpawnerManager&&) noexcept = delete;

	EnemySpawnerManager& operator=(const EnemySpawnerManager&) = delete;
	EnemySpawnerManager& operator=(EnemySpawnerManager&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void CreateEnemySpawner(const GameObjectRef& inOwner, const FVector& inLocation, const float& inSpawnRange, const EnemyID& inSpawnEnemyID, const int32& inSpawnCount, const int32& inLoop, const bool& inIsAggresive, const bool& inIsReward, const float& inMaxChaseRange, const float& inSearchRange);
	void ClearEnemySpawner();

public:
	std::vector<EnemySpawnerPtr>& GetEnemySpawners();

private:
	std::vector<EnemySpawnerPtr> mSpawners;
};