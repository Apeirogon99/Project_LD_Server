#include "pch.h"
#include "EnemySpawner.h"

EnemySpawner::EnemySpawner() : Actor(L"EnemySpawner"), mIsLoad(false), mMaxEnmeyCount(0), mCurEnemyCount(0), mEnemyID(0), mSpawnRange(0.0f), mLastSpawnCount(0)
{

}

EnemySpawner::~EnemySpawner()
{
	
}

void EnemySpawner::OnInitialization()
{
	SetTick(false, INFINITE);
}

void EnemySpawner::OnDestroy()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	for (EnemyCharacterPtr enemy : mEnemyCharacters)
	{
		if (enemy)
		{
			world->DestroyActor(enemy->GetGameObjectID());
			enemy.reset();
		}
	}
}

void EnemySpawner::OnTick(const int64 inDeltaTime)
{

}

bool EnemySpawner::IsValid()
{
	return mIsLoad == true;
}

void EnemySpawner::SetEnemySpawner(const int32 inEnemyID, const Stats& inStats, const int64 inMaxEnemyCount, const float inSpawnRange)
{
	mMaxEnmeyCount		= inMaxEnemyCount;
	mCurEnemyCount		= 0;

	mEnemyID			= inEnemyID;
	mEnemyStats			= inStats;
	mSpawnRange			= static_cast<float>((PI * ::pow(inSpawnRange, 2)));
	mLastSpawnCount		= 0;

	mEnemyCharacters.resize(inMaxEnemyCount);
	mIsLoad = true;
}

void EnemySpawner::OnAppearActor(ActorPtr inAppearActor)
{
}

void EnemySpawner::OnDisAppearActor(ActorPtr inDisappearActor)
{
}

void EnemySpawner::OnDestroyEnemy(const int64 inGameObjectID)
{
	for (int32 index = 0; index < mMaxEnmeyCount; ++index)
	{
		if (mEnemyCharacters[index]->GetGameObjectID() == inGameObjectID)
		{
			if (mEnemyCharacters[index])
			{
				mEnemyCharacters[index].reset();
				mCurEnemyCount--;
			}
			return;
		}
	}

	GameObjectLog(L"[OnDestroyEnemy] Invalid enemy id(%lld) in this spawner\n", inGameObjectID);
}

void EnemySpawner::ReSpawnCount()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (mMaxEnmeyCount - mCurEnemyCount == 0)
	{
		return;
	}

	if (mLastSpawnCount == 0)
	{
		mLastSpawnCount = world->GetWorldTime();
	}
	else
	{
		if (world->GetWorldTime() - mLastSpawnCount > MAX_RESPAWN_ENEMY_TIME)
		{
			for (int32 index = 0; index < mMaxEnmeyCount; ++index)
			{
				if (mEnemyCharacters[index])
				{
					world->DestroyActor(mEnemyCharacters[index]->GetGameObjectID());
					mEnemyCharacters[index].reset();
				}
			}

			mCurEnemyCount = 0;
		}
	}
}

bool EnemySpawner::IsLeftEnemy()
{
	return mCurEnemyCount == 0;
}

const Location EnemySpawner::GetRandomLocation()
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


EnemySpawnerManager::EnemySpawnerManager() : Actor(L"EnemySpawnerManager")
{
}

EnemySpawnerManager::~EnemySpawnerManager()
{
	
}

void EnemySpawnerManager::OnInitialization()
{
	SetTick(true, DEFAULT_TICK);

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == datas)
	{
		return;
	}

	CSVDatas spawnerDatas;
	datas->GetData(spawnerDatas, static_cast<uint8>(EGameDataType::EnemySpawner));

	const size_t datasSize = spawnerDatas.size() - 1;
	for (size_t index = 1; index < 2; ++index)
	{
		CSVRow row = spawnerDatas.at(index);

		int32 id	= std::stoi(row.at(0));
		float x		= std::stof(row.at(1));
		float y		= std::stof(row.at(2));
		float z		= std::stof(row.at(3));
		int32 count = std::stoi(row.at(4));

		ActorPtr		actor = world->SpawnActor<EnemySpawner>(this->GetGameObjectRef(), Location(x, y, z), Rotation(), Scale());
		EnemySpawnerPtr spawner = std::static_pointer_cast<EnemySpawner>(actor);
		spawner->SetEnemySpawner(id, datas->GetEnemyStat(id), count, 10.0f);

		mSpawners.push_back(spawner);
	}
}

void EnemySpawnerManager::OnDestroy()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	for (const auto& spawner : mSpawners)
	{
		world->DestroyActor(spawner->GetGameObjectID());
	}
}

void EnemySpawnerManager::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}


	for (auto& spawner : mSpawners)
	{
		if (false == spawner->IsValid())
		{
			continue;
		}

		if (spawner->IsLeftEnemy())
		{
			this->SpawnEnemys(spawner);
		}
		else
		{
			spawner->ReSpawnCount();
		}

	}
}

bool EnemySpawnerManager::IsValid()
{
	return mSpawners.size();
}

void EnemySpawnerManager::SpawnEnemys(EnemySpawnerPtr inSpanwer)
{
	switch (inSpanwer->GetEnemyID())
	{
	case 1:
		inSpanwer->SpawnEnemey<EnemySlime>();
		break;
	case 2:
		inSpanwer->SpawnEnemey<EnemySlime>();
		break;
	case 3:
		inSpanwer->SpawnEnemey<EnemySlime>();
		break;
	case 4:
		inSpanwer->SpawnEnemey<EnemySlime>();
		break;

	default:
		break;
	}
}
