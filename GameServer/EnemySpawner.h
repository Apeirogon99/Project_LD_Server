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
		SetTick(1000, true);
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
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) override {};
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) override {};

public:
	void SetEnemySpawner(WorldPtr inWorld, const int32 inEnemyID, const int64 inMaxEnemyCount, const float inSpawnRange);
	void OnDestroyEnemy(const int64 inGameObjectID);
	void SpawnEnemey();
	void ReSpawnCount();

public:
	bool IsLeftEnemy() { return mCurEnemyCount == 0; }
	const Protocol::SVector GetRandomLocation();

private:
	bool  mIsLoad;
	int64 mMaxEnmeyCount;
	int64 mCurEnemyCount;
	int32 mEnemyID;
	int64* mSpawnedEnemyIDs;
	Stats mEnemyStats;
	float mSpawnRange;
	int64 mLastSpawnCount;
	WorldPtr mWorld;
};

template<typename EnemyClass>
inline void EnemySpawner<EnemyClass>::SetEnemySpawner(WorldPtr inWorld, const int32 inEnemyID, const int64 inMaxEnemyCount, const float inSpawnRange)
{
	mWorld = inWorld;
	mEnemyID = inEnemyID;
	mMaxEnmeyCount = inMaxEnemyCount;
	mSpawnedEnemyIDs = new int64[mMaxEnmeyCount]();
	mSpawnRange = static_cast<float>((3.14 * ::pow((inSpawnRange/2), 2)));

	GameDatasPtr gameDatas = inWorld->GetGameDatas();
	CSVRow* enemyStats = gameDatas->PeekRow(static_cast<uint8>(EGameDataType::EnemyStat), mEnemyID);

	for (int32 index = 0; index < MAX_STATS_NUM; ++index)
	{
		mEnemyStats.GetStats()[index] = stof(enemyStats->at(index));
	}

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
		const Protocol::SVector newLocation = GetRandomLocation();
		EnemyCharacterPtr newEnemy = std::static_pointer_cast<EnemyCharacter>(world->CreateActor<EnemyClass>(newLocation, Protocol::SRotator()));
		if (nullptr == newEnemy)
		{
			return;
		}

		newEnemy->SetOwner(GetGameObjectRef());
		newEnemy->SetEnemeyID(this->mEnemyID);
		newEnemy->SetSpawnObjectID(this->GetGameObjectID());
		newEnemy->SetStats(this->mEnemyStats);
		newEnemy->SetSpawnLocation(newLocation);
		newEnemy->SetLocation(newLocation);

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
		mLastSpawnCount = mWorld->GetServiceTimeStamp();
	}
	else
	{
		static int64 MAX_DUARTION = 5000;
		if (mWorld->GetServiceTimeStamp() - mLastSpawnCount > MAX_DUARTION)
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
inline const Protocol::SVector EnemySpawner<EnemyClass>::GetRandomLocation()
{
	const Protocol::SVector& location = this->GetLocation();
	Protocol::SVector newSpawnLocation;

	float minX = location.x() - mSpawnRange;
	float maxX = location.x() + mSpawnRange;

	float minY = location.y() - mSpawnRange;
	float maxY = location.y() + mSpawnRange;

	std::random_device randDevice;
	std::mt19937 gen(randDevice());
	std::uniform_real_distribution<> distX(minX, maxX);
	std::uniform_real_distribution<> distY(minY, maxY);

	newSpawnLocation.set_x(static_cast<float>(distX(gen)));
	newSpawnLocation.set_y(static_cast<float>(distY(gen)));
	newSpawnLocation.set_z(location.z());
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