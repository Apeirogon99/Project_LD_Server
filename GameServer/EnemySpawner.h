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

class EnemySpawner : public Actor
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
	virtual void OnAppearActor(ActorPtr inAppearActor)			override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor)	override;
	void OnDestroyEnemy(const int64 inGameObjectID);

public:
	void SetEnemySpawner(const int32 inEnemyID, const Stats& inStats, const int64 inMaxEnemyCount, const float inSpawnRange);

	template<typename T>
	void SpawnEnemey();

public:
	void			ReSpawnCount();
	bool			IsLeftEnemy();
	const Location	GetRandomLocation();
	const Rotation	GetRandomRotation();

public:
	const int64 GetMaxEnemyCount()		const { return mMaxEnmeyCount; }
	const int64 GetCurrentEnemyCount()	const { return mCurEnemyCount; }
	const int64 GetEnemyID()			const { return mEnemyID; }

private:
	bool  mIsLoad;
	int64 mMaxEnmeyCount;
	int64 mCurEnemyCount;

	int32 mEnemyID;
	Stats mEnemyStats;
	float mSpawnRange;
	int64 mLastSpawnCount;

	std::vector<EnemyCharacterPtr> mEnemyCharacters;
};

template<typename EnemyClass>
inline void EnemySpawner::SpawnEnemey()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	for (int32 index = 0; index < mMaxEnmeyCount; ++index)
	{
		const Location	newLocation = GetRandomLocation();
		const Rotation	newRoation	= GetRandomRotation();
		const Scale		newScale	= Scale(1.0f, 1.0f, 1.0f);

		EnemyCharacterPtr newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<EnemyClass>(this->GetGameObjectRef(), newLocation, newRoation, newScale));
		if (nullptr == newEnemy)
		{
			return;
		}

		newEnemy->SetEnemeyID(this->mEnemyID);
		newEnemy->SetActorType(static_cast<uint8>(EActorType::Enemy));
		newEnemy->SetSpawnObjectID(this->GetGameObjectID());
		newEnemy->SetEnemyStats(this->mEnemyStats);
		newEnemy->SetRecoveryLocation(newLocation);

		const int64 enemyGameObjectID = newEnemy->GetGameObjectID();
		//GameObjectLog(L"[SpawnEnemey] spawn enemy(%lld)\n", enemyGameObjectID);
		mEnemyCharacters[index] = newEnemy;
	}

	mLastSpawnCount = 0;
	mCurEnemyCount = mMaxEnmeyCount;
}

class EnemySpawnerManager : public Actor
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

protected:
	void SpawnEnemys(EnemySpawnerPtr inSpanwer);

private:
	std::vector<EnemySpawnerPtr> mSpawners;
	//KD-Tree
};
