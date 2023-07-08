#include "pch.h"
#include "EnemySlime.h"

EnemySlime::EnemySlime() : EnemyCharacter(L"Enemy::Slime")
{
}

EnemySlime::~EnemySlime()
{
}

void EnemySlime::OnInitialization()
{
	SetTick(true, DEFAULT_TICK);
	mStateManager.SetEnemy(GetEnemyCharacterRef());
	mStateManager.SetState(EStateType::State_Idle);
}

void EnemySlime::OnDestroy()
{
	EnemySpawnerPtr spawner = std::static_pointer_cast<EnemySpawner>(GetOwner().lock());
	if (nullptr == spawner)
	{
		return;
	}
	spawner->OnDestroyEnemy(GetGameObjectID());
}

void EnemySlime::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	mStateManager.UpdateState(inDeltaTime);

}

bool EnemySlime::IsValid()
{
	return GetEnemyID() != 0;
}