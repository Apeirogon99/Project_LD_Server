#include "pch.h"
#include "IStateEvent.h"

//==========================//
//		     IDLE			//
//==========================//

void IdleState::Enter(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	mIdleToRoundTime = 0;
}

void IdleState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	mIdleToRoundTime += inDeltaTime;
	if (mIdleToRoundTime >= IDLE_TO_ROUND_TIME)
	{
		enemy->GetStateManager().SetState(EStateType::State_Round);
	}
}

void IdleState::Exit(EnemyCharacterRef inEnemy)
{

}

//==========================//
//		     ROUND			//
//==========================//

void RoundState::Enter(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	EnemySpawnerPtr spawner = std::static_pointer_cast<EnemySpawner>(enemy->GetOwner().lock());
	if (nullptr == spawner)
	{
		return;
	}

	float velocity = enemy->GetEnemyStatsComponent().GetCurrentStats().GetMovementSpeed();
	enemy->SetVelocity(velocity, velocity, velocity);

	Location currentLocation	= enemy->GetLocation();
	Location nextLocation		= spawner->GetRandomLocation();
	const float collisionRadius = enemy->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();

	enemy->GetMovementComponent().SetNewDestination(enemy->GetActorPtr(), currentLocation, nextLocation, world->GetWorldTime(), collisionRadius);
	enemy->SetRecoveryLocation(nextLocation);
	enemy->OnMovementEnemy();

	//nextLocation.ToString();
}

void RoundState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	if (false == enemy->GetMovementComponent().Update(enemy->GetActorPtr(), 10.0f))
	{
		enemy->GetStateManager().SetState(EStateType::State_Idle);
	}

}

void RoundState::Exit(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	enemy->SetVelocity(0.0f, 0.0f, 0.0f);

}

//==========================//
//		   RECOVERY			//
//==========================//

void RecoveryState::Enter(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	float velocity = enemy->GetEnemyStatsComponent().GetCurrentStats().GetMovementSpeed();
	enemy->SetVelocity(velocity, velocity, velocity);

	Location currentLocation	= enemy->GetLocation();
	Location recoveryLocation	= enemy->GetRecoveryLocation();
	const float collisionRadius = enemy->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();
	this->mCurrentLocation		= enemy->GetLocation();
	this->mMaxDistanceLength	= FVector::Distance2D(this->mCurrentLocation, recoveryLocation);

	enemy->GetMovementComponent().SetNewDestination(enemy->GetActorPtr(), currentLocation, recoveryLocation, world->GetWorldTime(), collisionRadius);
	enemy->OnMovementEnemy();
}

void RecoveryState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	if (false == enemy->GetMovementComponent().Update(enemy->GetActorPtr(), MAX_LOCATION_DISTANCE))
	{
		enemy->GetStateManager().SetState(EStateType::State_Idle);
	}

	const float curHealth = enemy->GetEnemyStatsComponent().GetCurrentStats().GetHealth();
	const float maxHealth = enemy->GetEnemyStatsComponent().GetMaxStats().GetHealth();

	const float distanceLenght = FVector::Distance2D(enemy->GetLocation(), mCurrentLocation);
	const float disatncePercent = distanceLenght / this->mMaxDistanceLength;
	
	const float recoveryHealth = curHealth + (maxHealth - curHealth) * disatncePercent;
	enemy->GetEnemyStatsComponent().UpdateCurrentStat(EStatType::Stat_Health, recoveryHealth);
}

void RecoveryState::Exit(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	const float fullHealth = enemy->GetEnemyStatsComponent().GetMaxStats().GetHealth();
	enemy->GetEnemyStatsComponent().UpdateCurrentStat(EStatType::Stat_Health, fullHealth);
}


//==========================//
//		     CHASE			//
//==========================//

void ChaseState::Enter(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	ActorPtr aggroActor = enemy->GetAggroActor().lock();
	if (nullptr == aggroActor)
	{
		enemy->GetStateManager().SetState(EStateType::State_Recovery);
		return;
	}

	float velocity = enemy->GetEnemyStatsComponent().GetCurrentStats().GetMovementSpeed();
	enemy->SetVelocity(velocity, velocity, velocity);

	Location currentLocation = enemy->GetLocation();
	Location aggroLocation = aggroActor->GetLocation();
	const int64 worldTime = world->GetWorldTime();

	const float collisionRadius = enemy->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();
	const float enemyAttackRange = enemy->GetEnemyStatsComponent().GetCurrentStats().GetRange() + collisionRadius;
	enemy->GetMovementComponent().SetNewDestination(enemy->GetActorPtr(), currentLocation, aggroLocation, world->GetWorldTime(), enemyAttackRange);
	enemy->OnMovementEnemy();

	mChaseToRecoveryTime = 0;
}

void ChaseState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	ActorPtr aggroActor = enemy->GetAggroActor().lock();
	if (nullptr == aggroActor)
	{
		enemy->GetStateManager().SetState(EStateType::State_Recovery);
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	const float collisionRadius = enemy->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();
	const float enemyAttackRange = enemy->GetEnemyStatsComponent().GetCurrentStats().GetRange() + collisionRadius;
	if (false == enemy->GetMovementComponent().Update(enemy->GetActorPtr(), enemyAttackRange))
	{
		enemy->GetStateManager().SetState(EStateType::State_Attack);
		return;
	}

	float velocity = enemy->GetEnemyStatsComponent().GetCurrentStats().GetMovementSpeed();
	enemy->SetVelocity(velocity, velocity, velocity);

	Location currentLocation	= enemy->GetLocation();
	Location aggroLocation		= aggroActor->GetLocation();
	const int64 worldTime		= world->GetWorldTime();
	enemy->GetMovementComponent().SetNewDestination(enemy->GetActorPtr(), currentLocation, aggroLocation, world->GetWorldTime(), enemyAttackRange);

	mChaseToRecoveryTime += inDeltaTime;
	if (mChaseToRecoveryTime >= CHASE_TO_RECOVERY_TIME)
	{
		enemy->GetStateManager().SetState(EStateType::State_Recovery);
		//enemy->SetAggroPlayer(nullptr);
	}

}

void ChaseState::Exit(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
}

//==========================//
//		    ATTACK			//
//==========================//

void AttackState::Enter(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	ActorPtr aggroActor = enemy->GetAggroActor().lock();
	if (nullptr == aggroActor)
	{
		enemy->GetStateManager().SetState(EStateType::State_Recovery);
		return;
	}

	if (false == aggroActor->IsValid())
	{
		enemy->GetStateManager().SetState(EStateType::State_Recovery);
		return;
	}

	StatsComponent& stat	= enemy->GetEnemyStatsComponent();
	const float damage		= stat.GetCurrentStats().GetAttackDamage();

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	enemy->GetAutoAttackComponent().DoMeleeAutoAttack(enemy->GetActorPtr(), aggroActor, damage);
}

void AttackState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{

}

void AttackState::Exit(EnemyCharacterRef inEnemy)
{
}

//==========================//
//		     HIT			//
//==========================//

void HitState::Enter(EnemyCharacterRef inEnemy)
{
	mHitToChaseTime = 0;
}

void HitState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	mHitToChaseTime += inDeltaTime;
	if (mHitToChaseTime >= HIT_TO_CHASE_TIME)
	{
		enemy->GetStateManager().SetState(EStateType::State_Chase);
	}
}

void HitState::Exit(EnemyCharacterRef inEnemy)
{
}

//==========================//
//		    STUN			//
//==========================//

void StunState::Enter(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	enemy->SetVelocity(0.0f, 0.0f, 0.0f);

	mStunToChaseTime	= 0;
	mStunTime			= 0;
}

void StunState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	mStunToChaseTime += inDeltaTime;
	if (mStunToChaseTime >= mStunTime)
	{
		enemy->GetStateManager().SetState(EStateType::State_Chase);
	}
}

void StunState::Exit(EnemyCharacterRef inEnemy)
{
}

void StunState::SetStunTime(const int64 inStunTime)
{
	mStunTime = inStunTime;
}


//==========================//
//		    DEATH			//
//==========================//

void DeathState::Enter(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	enemy->SetVelocity(0.0f, 0.0f, 0.0f);

	const int64 worldTime = world->GetWorldTime();
	const int64 deathTime = worldTime + 2670;
	enemy->PushTask(deathTime, &Actor::OnDeath);
}

void DeathState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
}

void DeathState::Exit(EnemyCharacterRef inEnemy)
{
}

void StateManager::StateChangeDebugPrint(const EStateType& inOldType, const EStateType& inNewType)
{
	wprintf(L"[ID::%2lld][ChangeState] [OLD::%ws] -> [NEW::%ws]\n", mEnemy.lock()->GetGameObjectID(), ToStringState(inOldType).c_str(), ToStringState(inNewType).c_str());
}