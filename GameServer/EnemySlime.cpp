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
	SetTick(true, ENEMY_STATE_TICK);
	mStateManager.SetEnemy(GetEnemyCharacterRef());
	mStateManager.SetState(EStateType::State_Idle);
}

void EnemySlime::OnDestroy()
{
	std::shared_ptr<EnemySpawner<EnemySlime>> spawner = std::static_pointer_cast<EnemySpawner<EnemySlime>>(GetOwner().lock());
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