#include "pch.h"
#include "EnemySpawner.h"

EnemySpawnerManager::EnemySpawnerManager() : Actor(L"EnemySpawnerManager")
{
}

EnemySpawnerManager::~EnemySpawnerManager()
{
}

void EnemySpawnerManager::OnInitialization()
{
	SetTick(DEFAULT_SYNC_TICK, true);

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	ActorPtr actor = world->CreateActor<EnemySpawner<EnemySlime>>(Protocol::SVector(), Protocol::SRotator());
	std::shared_ptr<EnemySpawner<EnemySlime>> enemySlimeSpawner = std::static_pointer_cast<EnemySpawner<EnemySlime>>(actor);
	enemySlimeSpawner->SetEnemySpawner(std::static_pointer_cast<World>(shared_from_this()), 1, 5, 20.0f);
	enemySlimeSpawner->SetLocation(100.0f, 100.0f, 500.0f);
	mSpawners.push_back(actor->GetGameObjectID());
}

void EnemySpawnerManager::OnDestroy()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	for (const int64 spawnerObjectID : mSpawners)
	{
		world->DestroyActor(spawnerObjectID);
	}
}

void EnemySpawnerManager::OnTick(const int64 inDeltaTime)
{
	CheackSpawners();
}

bool EnemySpawnerManager::IsValid()
{
	return false;
}

void EnemySpawnerManager::CheackSpawners()
{

}
