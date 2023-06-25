#pragma once

template<typename EnemyClass>
class EnemySpawner : public Actor
{
public:
	EnemySpawner() : Actor(L"[EnemySpawner]"), mIsLoad(false), mMaxEnmeyCount(0), mCurEnemyCount(0), mEnemyID(0), mSpawnedEnemyIDs(nullptr), mSpawnRange(0.0f), mLastSpawnCount(0){}
	~EnemySpawner() {}

	EnemySpawner(const EnemySpawner&) = delete;
	EnemySpawner(EnemySpawner&&) noexcept = delete;

	EnemySpawner& operator=(const EnemySpawner&) = delete;
	EnemySpawner& operator=(EnemySpawner&&) noexcept = delete;

public:
	virtual void Initialization()	override
	{
		
	}

	virtual void Destroy()			override
	{
		if (mSpawnedEnemyIDs)
		{
			delete[] mSpawnedEnemyIDs;
		}
		mSpawnedEnemyIDs = nullptr;
	}

	virtual void Tick()				override
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
	void SetEnemySpawner(WorldPtr inWorld, const int32 inEnemyID, const int32 inMaxEnemyCount, const float inSpawnRange);
	void OnDestroyEnemy(const int64 inGameObjectID);
	void SpawnEnemey();
	void ReSpawnCount();

public:
	bool IsLeftEnemy() { return mCurEnemyCount == 0; }
	const Protocol::SVector GetRandomSpawnLocation();

private:
	bool  mIsLoad;
	int32 mMaxEnmeyCount;
	int32 mCurEnemyCount;
	int32 mEnemyID;
	int64* mSpawnedEnemyIDs;
	Stats mEnemyStats;
	float mSpawnRange;
	int64 mLastSpawnCount;
	WorldPtr mWorld;
};

template<typename EnemyClass>
inline void EnemySpawner<EnemyClass>::SetEnemySpawner(WorldPtr inWorld, const int32 inEnemyID, const int32 inMaxEnemyCount, const float inSpawnRange)
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
	mCurEnemyCount--;
}

template<typename EnemyClass>
inline void EnemySpawner<EnemyClass>::SpawnEnemey()
{

	for (int32 index = 0; index < mMaxEnmeyCount; ++index)
	{
		const Protocol::SVector newLocation = GetRandomSpawnLocation();
		EnemyCharacterPtr newEnemy = std::static_pointer_cast<EnemyCharacter>(mWorld->CreateActor<EnemyClass>(newLocation, Protocol::SRotator()));
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

		wprintf(L"[%lld]\t[%ld]\t(%f\t:%f\t:%f)\n", newEnemy->GetGameObjectID(), newEnemy->GetEnemyID(), newEnemy->GetLocation().x(), newEnemy->GetLocation().y(), newEnemy->GetLocation().z());

		mSpawnedEnemyIDs[index] = newEnemy->GetGameObjectID();
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
inline const Protocol::SVector EnemySpawner<EnemyClass>::GetRandomSpawnLocation()
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
