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
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	this->mCapsuleCollisionComponent.SetOwner(this->GetActorRef());
	this->mCapsuleCollisionComponent.SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos infos;
	infos.push_back(AttackInfo(0, 550, 1700, FVector(20.0f, 20.0f, 20.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Ranged, infos);
}

void EnemyArcherSkeleton::OnAutoAttackShot(ActorPtr inVictim)
{
	//NONE
}

void EnemyArcherSkeleton::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
{
	//TODO : Arrow
}

void EnemyArcherSkeleton::OnAutoAttackOver()
{
	this->mAutoAttackComponent.OnOverAutoAttack();

	if (false == this->IsDeath())
	{
		this->mStateManager.SetState(EStateType::State_Chase);
	}
}

void EnemyArcherSkeleton::OnReward()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	Location location = this->GetLocation();

	AItemPtr money = std::static_pointer_cast<AItem>(world->SpawnActor<AItem>(world->GetGameObjectRef(), Random::GetRandomVectorInRange2D(location, 100.0f), FRotator(), FVector()));
	money->SetItemCode(171);
	money->SetAmount(static_cast<int32>(Random::GetNormalDistribution(100, 0.6)));

	if (true == Random::GetRandomProbability(1.0, 100.0, 40.0))
	{
		AItemPtr bone = std::static_pointer_cast<AItem>(world->SpawnActor<AItem>(world->GetGameObjectRef(), Random::GetRandomVectorInRange2D(location, 100.0f), FRotator(), FVector()));
		bone->SetItemCode(173);
	}
}
