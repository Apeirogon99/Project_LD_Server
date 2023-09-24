#include "pch.h"
#include "EnemySpawner.h"

EnemySpawner::EnemySpawner() : GameObject(L"EnemySpawner"), mIsLoad(false), mMaxEnmeyCount(0), mCurEnemyCount(0), mEnemyID(0), mSpawnRange(0.0f), mRespawnTime(0), mMaxChaseRange(0.0f), mMaxSearchRange(0.0f)
{

}

EnemySpawner::~EnemySpawner()
{
	
}

void EnemySpawner::OnInitialization()
{
	SetTick(true, GAME_TICK);
}

void EnemySpawner::OnDestroy()
{
	this->ClearEnemy();
}

void EnemySpawner::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	if (true == this->IsRespawn())
	{
		this->SpawnEnemy();
		mRespawnTime = 0;
	}
}

bool EnemySpawner::IsValid()
{
	return mIsLoad == true;
}

void EnemySpawner::SetEnemySpawner(const FVector& inLocation, const int32& inEnemyID, const float& inSpawnRange, const int32& inSpawnCount, const int32& inSpawnLoop, const bool& inIsAggresive, const bool& inIsReward, const float& inMaxChaseRange, const float& inSearchRange)
{
	EnemySpawnerManagerPtr manager = std::static_pointer_cast<EnemySpawnerManager>(this->GetOwner().lock());
	if (nullptr == manager)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(manager->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	GameDatasPtr data = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == data)
	{
		return;
	}

	mMaxEnmeyCount	= inSpawnCount;
	mCurEnemyCount	= 0;

	mEnemyID		= inEnemyID;
	mEnemyStats		= data->GetEnemyStat(inEnemyID);
	mSpawnRange		= inSpawnRange;
	mMaxChaseRange	= inMaxChaseRange;
	mMaxSearchRange = inSearchRange;
	mSpawnLoop		= inSpawnLoop;
	mIsAggresive	= inIsAggresive;
	mIsReward		= inIsReward;

	mLocation		= inLocation;

	mIsLoad			= true;

	this->SpawnEnemy();
}

void EnemySpawner::SpawnEnemy()
{
	if (mSpawnLoop == 0)
	{
		return;
	}

	EnemySpawnerManagerPtr manager = std::static_pointer_cast<EnemySpawnerManager>(this->GetOwner().lock());
	if (nullptr == manager)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(manager->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& nextWorldTime = world->GetNextWorldTime();

	for (int32 count = 0; count < mMaxEnmeyCount; ++count)
	{
		ActorPtr newActor = this->SpawnTemplate();
		if (nullptr == newActor)
		{
			return;
		}

		EnemyCharacterPtr newEnemy = std::static_pointer_cast<EnemyCharacter>(newActor);
		if (nullptr == newEnemy)
		{
			return;
		}
		newEnemy->SetActorType(static_cast<uint8>(EActorType::Enemy));
		newEnemy->SetSpawnObjectID(this->GetGameObjectID());
		newEnemy->SetEnemeyID(this->mEnemyID);
		newEnemy->SetEnemyStats(this->mEnemyStats);
		newEnemy->SetRecoveryLocation(this->mLocation);
		newEnemy->SetReward(this->mIsReward);
		newEnemy->SetAggressive(this->mIsAggresive);
		newEnemy->SetMaxChaseRange(this->mMaxChaseRange);
		newEnemy->SetMaxSearchRange(this->mMaxSearchRange);

		if (this->mIsAggresive)
		{
			newEnemy->GetStateManager().SetState(EStateType::State_Search);
		}
		else
		{
			newEnemy->GetStateManager().SetState(EStateType::State_Idle);
		}
		//newEnemy->PushTask(nextWorldTime, &EnemyCharacter::OnBeginSpawn);

		mEnemyCharacters.emplace_back(newEnemy);

		mCurEnemyCount++;
	}

	if (mSpawnLoop != -1)
	{
		mSpawnLoop--;
	}
}

void EnemySpawner::NotifyDestroyEnemy(const int64& inGameObjectID)
{
	auto iter = mEnemyCharacters.begin();
	for (iter; iter != mEnemyCharacters.end(); iter++)
	{
		EnemyCharacterPtr enemy = *iter;
		if (enemy->GetGameObjectID() == inGameObjectID)
		{
			mEnemyCharacters.erase(iter);
			mCurEnemyCount--;
			break;
		}
	}
}

void EnemySpawner::ClearEnemy()
{
	EnemySpawnerManagerPtr manager = std::static_pointer_cast<EnemySpawnerManager>(this->GetOwner().lock());
	if (nullptr == manager)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(manager->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	for (auto enemy : mEnemyCharacters)
	{
		if (enemy)
		{
			enemy->SetReward(false);
			world->PushTask(world->GetNextWorldTime(), &World::DestroyActor, enemy->GetGameObjectID());
		}
	}

	mCurEnemyCount = 0;
}

bool EnemySpawner::IsRespawn()
{
	EnemySpawnerManagerPtr manager = std::static_pointer_cast<EnemySpawnerManager>(this->GetOwner().lock());
	if (nullptr == manager)
	{
		return false;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(manager->GetOwner().lock());
	if (nullptr == world)
	{
		return false;
	}
	const int64& worldTime = world->GetWorldTime();

	if (mMaxEnmeyCount - mCurEnemyCount == 0)
	{
		return false;
	}

	if (mRespawnTime == 0)
	{
		mRespawnTime = worldTime;
	}

	if (worldTime - mRespawnTime <= MAX_RESPAWN_ENEMY_TIME)
	{
		return false;
	}

	for (auto enemy : mEnemyCharacters)
	{
		if (enemy)
		{
			EStateType state = enemy->GetStateManager().GetCurrentStateType();
			if (state != EStateType::State_Idle && state != EStateType::State_Round)
			{
				mRespawnTime = 0;
				return false;
			}
		}
	}
	this->ClearEnemy();
	
	return true;
}

ActorPtr EnemySpawner::SpawnTemplate()
{
	EnemySpawnerManagerPtr manager = std::static_pointer_cast<EnemySpawnerManager>(this->GetOwner().lock());
	if (nullptr == manager)
	{
		return nullptr;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(manager->GetOwner().lock());
	if (nullptr == world)
	{
		return nullptr;
	}

	ActorPtr newActor = nullptr;
	switch (static_cast<EnemyID>(this->mEnemyID))
	{
	case EnemyID::Enemy_Slime:
		newActor = world->SpawnActor<EnemySlime>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Nomal_Skeleton:
		newActor = world->SpawnActor<EnemyNomalSkeleton>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Warrior_Skeleton:
		newActor = world->SpawnActor<EnemyWarriorSkeleton>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Archer_Skeleton:
		world->SpawnActor<EnemyArcherSkeleton>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Dark_Skeleton:
		newActor = world->SpawnActor<EnemyDarkSkeleton>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Dark_Knight:
		newActor = world->SpawnActor<EnemyDarkKnight>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Lich_Phase1:
		newActor = world->SpawnActor<EnemyRichPhase1>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Lich_Phase2:
		newActor = world->SpawnActor<EnemyRichPhase2>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Lich_Phase3:
		newActor = world->SpawnActor<EnemyRichPhase3>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	case EnemyID::Enemy_Lich_Life_Vessle:
		newActor = world->SpawnActor<EnemyDarkKnight>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(mLocation, mSpawnRange), Rotation(), Scale(1.0f, 1.0f, 1.0f));
		break;
	default:
		break;
	}

	return newActor;
}

///////////////////////////////////
//
//	EnemySpawnerManager
//
///////////////////////////////////

EnemySpawnerManager::EnemySpawnerManager() : GameObject(L"EnemySpawnerManager")
{
}

EnemySpawnerManager::~EnemySpawnerManager()
{
	
}

void EnemySpawnerManager::OnInitialization()
{
}

void EnemySpawnerManager::OnDestroy()
{
	this->ClearEnemySpawner();
}

void EnemySpawnerManager::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}
}

bool EnemySpawnerManager::IsValid()
{
	return mSpawners.size();
}

void EnemySpawnerManager::CreateEnemySpawner(const FVector& inLocation, const float& inSpawnRange, const EnemyID& inSpawnEnemyID, const int32& inSpawnCount, const int32& inLoop, const bool& inIsAggresive, const bool& inIsReward, const float& inMaxChaseRange, const float& inSearchRange)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	GameTaskPtr task = std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());
	if (nullptr == task)
	{
		return;
	}

	EnemySpawnerPtr newSpawner = std::make_shared<EnemySpawner>();
	if (nullptr == newSpawner)
	{
		return;
	}
	newSpawner->SetOwner(this->GetGameObjectRef());
	task->PushTask(newSpawner->GetGameObjectPtr());

	newSpawner->SetEnemySpawner(inLocation, static_cast<int32>(inSpawnEnemyID), inSpawnRange, inSpawnCount, inLoop, inIsAggresive , inIsReward, inMaxChaseRange, inSearchRange);

	mSpawners.push_back(newSpawner);
}

void EnemySpawnerManager::ClearEnemySpawner()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	for (const auto& spawner : mSpawners)
	{
		world->DestroyActor(spawner->GetGameObjectID());
	}


	mSpawners.clear();
}

std::vector<EnemySpawnerPtr>& EnemySpawnerManager::GetEnemySpawners()
{
	return mSpawners;
}
