#include "pch.h"
#include "EnemyArcherSkeleton.h"

EnemyArcherSkeleton::EnemyArcherSkeleton() : EnemyCharacter(L"Enemy::ArcherSkeleton")
{
}

EnemyArcherSkeleton::~EnemyArcherSkeleton()
{
}

void EnemyArcherSkeleton::OnInitialization()
{
	SetTick(true, SYSTEM_TICK);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	this->mStatsComponent.SetSyncTime(DEFAULT_TICK);

	this->mCapsuleCollisionComponent.SetOwner(this->GetActorRef());
	this->mCapsuleCollisionComponent.SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), DEFAULT_TICK);

	AttackInfos infos;
	infos.push_back(AttackInfo(550, 0, 1600, FVector(20.0f, 20.0f, 20.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Ranged, infos);
}

void EnemyArcherSkeleton::OnAutoAttackShot(ActorPtr inVictim)
{
}

void EnemyArcherSkeleton::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
{
}

void EnemyArcherSkeleton::OnAutoAttackOver()
{
	this->mAutoAttackComponent.OnOverAutoAttack();
	this->mStateManager.SetState(EStateType::State_Chase);
}
