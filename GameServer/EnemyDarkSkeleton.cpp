#include "pch.h"
#include "EnemyDarkSkeleton.h"

EnemyDarkSkeleton::EnemyDarkSkeleton()
{
}

EnemyDarkSkeleton::~EnemyDarkSkeleton()
{
}

void EnemyDarkSkeleton::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);
	this->mSpawnParticle = static_cast<int32>(ESkillID::Skill_Rich_Rise_DarkSkeleton);
	this->mSpawnParticleDelay = 3000;

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos infos;
	infos.push_back(AttackInfo(0, 450, 1700, FVector(50.0f, 80.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Melee, infos);
}
