#pragma once

template<typename EnemyClass>
class EnemySpawner : public Actor
{
public:
	EnemySpawner() : Actor(L"EnemySpawner"), mIsLoad(false), mMaxEnmeyCount(0), mCurEnemyCount(0), mEnemyID(0), mSpawnedEnemyIDs(nullptr), mSpawnRange(0.0f), mLastSpawnCount(0){}
	~EnemySpawner() {}

	EnemySpawner(const EnemySpawner&) = delete;
	EnemySpawner(EnemySpawner&&) noexcept = delete;

	EnemySpawner& operator=(const EnemySpawner&) = delete;
	EnemySpawner& operator=(EnemySpawner&&) noexcept = delete;

public:
	virtual void OnInitialization()	override
	{
		SetTick(false, DEFAULT_TICK);
	}

	virtual void OnDestroy() override
	{
		for (int64 enemyObjectID = 0; enemyObjectID < mMaxEnmeyCount; ++enemyObjectID)
		{
			mWorld->DestroyActor(mSpawnedEnemyIDs[enemyObjectID]);
		}

		if (mSpawnedEnemyIDs)
		{
			delete[] mSpawnedEnemyIDs;
		}
		mSpawnedEnemyIDs = nullptr;
	}

	virtual void OnTick(const int64 inDeltaTime) override
	{

		if (false == IsValid())
		{
			return;
		}

		if (IsLeftEnemy())
		{
			SpawnEnemey();
		}
		else
		{
			ReSpawnCount();
		}

	}
	virtual bool IsValid()			override 
	{ 
		return mIsLoad == true; 
	}

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) {};
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) {};

public:
	void SetEnemySpawner(GameWorldPtr inWorld, const int32 inEnemyID, const int64 inMaxEnemyCount, const float inSpawnRange);
	void OnDestroyEnemy(const int64 inGameObjectID);
	void SpawnEnemey();
	void ReSpawnCount();

public:
	bool IsLeftEnemy() { return mCurEnemyCount == 0; }
	const Location GetRandomLocation();

private:
	bool  mIsLoad;
	int64 mMaxEnmeyCount;
	int64 mCurEnemyCount;
	int32 mEnemyID;
	int64* mSpawnedEnemyIDs;
	Stats mEnemyStats;
	float mSpawnRange;
	int64 mLastSpawnCount;
	GameWorldPtr mWorld;
};

template<typename EnemyClass>
inline void EnemySpawner<EnemyClass>::SetEnemySpawner(GameWorldPtr inWorld, const int32 inEnemyID, const int64 inMaxEnemyCount, const float inSpawnRange)
{
	mWorld = inWorld;
	mEnemyID = inEnemyID;
	mMaxEnmeyCount = inMaxEnemyCount;
	mSpawnedEnemyIDs = new int64[mMaxEnmeyCount]();
	mSpawnRange = static_cast<float>((3.14 * ::pow((inSpawnRange/2), 2)));

	GameDatasPtr gameDatas = std::static_pointer_cast<GameDatas>(inWorld->GetDatas());
	mEnemyStats = gameDatas->GetEnemyStat(mEnemyID);
	mIsLoad = true;
}

template<typename EnemyClass>
inline void EnemySpawner<EnemyClass>::OnDestroyEnemy(const int64 inGameObjectID)
{
	for (int32 index = 0; index < mMaxEnmeyCount; ++index)
	{
		if (mSpawnedEnemyIDs[index] == inGameObjectID)
		{
			mSpawnedEnemyIDs[index] = 0;
			mCurEnemyCount--;
			return;
		}
	}

	GameObjectLog(L"[OnDestroyEnemy] Invalid enemy id(%lld) in this spawner\n", inGameObjectID);
}

template<typename EnemyClass>
inline void EnemySpawner<EnemyClass>::SpawnEnemey()
{

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	for (int32 index = 0; index < mMaxEnmeyCount; ++index)
	{
		const Location newLocation = GetRandomLocation();
		const Rotation newRoation = Rotation();

		EnemyCharacterPtr newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<EnemyClass>(newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f)));
		if (nullptr == newEnemy)
		{
			return;
		}

		newEnemy->SetOwner(this->GetGameObjectRef());
		newEnemy->SetEnemeyID(this->mEnemyID);
		newEnemy->SetSpawnObjectID(this->GetGameObjectID());
		newEnemy->SetEnemyStats(this->mEnemyStats);
		newEnemy->SetRecoveryLocation(newLocation);
		newEnemy->SetLocation(newLocation);
		newEnemy->SetRotation(newRoation);

		const int64 enemyGameObjectID = newEnemy->GetGameObjectID();
		GameObjectLog(L"[SpawnEnemey] spawn enemy(%lld)\n", enemyGameObjectID);
		mSpawnedEnemyIDs[index] = enemyGameObjectID;
	}

	mLastSpawnCount = 0;
	mCurEnemyCount = mMaxEnmeyCount;
}

template<typename EnemyClass>
inline void EnemySpawner<EnemyClass>::ReSpawnCount()
{
	if (mMaxEnmeyCount - mCurEnemyCount == 0)
	{
		return;
	}

	if (mLastSpawnCount == 0)
	{
		mLastSpawnCount = mWorld->GetWorldTime();
	}
	else
	{
		static int64 MAX_DUARTION = 5000;
		if (mWorld->GetWorldTime() - mLastSpawnCount > MAX_DUARTION)
		{
			for (int32 index = 0; index < mMaxEnmeyCount; ++index)
			{
				mWorld->DestroyActor(mSpawnedEnemyIDs[index]);
			}

			mCurEnemyCount = 0;
		}
	}

}

template<typename EnemyClass>
inline const Location EnemySpawner<EnemyClass>::GetRandomLocation()
{
	const Location& spawnerlocation = this->GetLocation();
	Location newSpawnLocation;

	float minX = spawnerlocation.GetX() - mSpawnRange;
	float maxX = spawnerlocation.GetX() + mSpawnRange;

	float minY = spawnerlocation.GetY() - mSpawnRange;
	float maxY = spawnerlocation.GetY() + mSpawnRange;

	std::random_device randDevice;
	std::mt19937 gen(randDevice());
	std::uniform_real_distribution<> distX(minX, maxX);
	std::uniform_real_distribution<> distY(minY, maxY);

	newSpawnLocation.SetX(static_cast<float>(distX(gen)));
	newSpawnLocation.SetY(static_cast<float>(distY(gen)));
	newSpawnLocation.SetZ(spawnerlocation.GetZ());
	return newSpawnLocation;
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
	void CheackSpawners();

private:
	std::vector<int64> mSpawners;
};