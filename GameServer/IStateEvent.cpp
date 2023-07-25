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
	const float enemyAttackRange = enemy->GetEnemyStatsComponent().GetCurrentStats().GetRange();
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
	const float enemyAttackRange = enemy->GetEnemyStatsComponent().GetCurrentStats().GetRange();
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
		return;
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

	enemy->GetAggroActor().reset();
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


	enemy->SetVelocity(0.0f, 0.0f, 0.0f);

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
	const int64 deathTime = worldTime + 1000;
	enemy->PushTask(deathTime, &Actor::OnDeath);
}

void DeathState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
}

void DeathState::Exit(EnemyCharacterRef inEnemy)
{
}

StateManager::StateManager() : mOldState(EStateType::State_Unspecified), mCurrentState(EStateType::State_Unspecified)
{
	mStateTypes.insert(std::make_pair(EStateType::State_Idle, static_cast<IStateEvent*>(new IdleState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Round, static_cast<IStateEvent*>(new RoundState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Recovery, static_cast<IStateEvent*>(new RecoveryState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Chase, static_cast<IStateEvent*>(new ChaseState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Attack, static_cast<IStateEvent*>(new AttackState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Hit, static_cast<IStateEvent*>(new HitState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Death, static_cast<IStateEvent*>(new DeathState())));
}

StateManager::~StateManager()
{
	for (std::map<EStateType, IStateEvent*>::iterator state = mStateTypes.begin(); state != mStateTypes.end(); state++)
	{
		delete state->second;
	}
}

void StateManager::SetEnemy(EnemyCharacterRef inEnemy)
{
	mEnemy = inEnemy;
}

void StateManager::SetState(const EStateType& inStateType)
{
	if (mCurrentState == EStateType::State_Unspecified)
	{
		mCurrentState = inStateType;
		EnterState();
		return;
	}

	mOldState = mCurrentState;

	IStateEvent* oldState = mStateTypes.at(mCurrentState);
	oldState->Exit(mEnemy);

	mCurrentState = inStateType;

	IStateEvent* newState = mStateTypes.at(mCurrentState);
	newState->Enter(mEnemy);

	EnemyCharacterPtr enemy = mEnemy.lock();
	if (nullptr != enemy)
	{
		mEnemy.lock()->DetectChangeEnemy();
	}
	StateChangeDebugPrint(mOldState, mCurrentState);
}

const EStateType& StateManager::GetCurrentStateType() const
{
	return mCurrentState;
}

void StateManager::StateChangeDebugPrint(const EStateType& inOldType, const EStateType& inNewType)
{
	wprintf(L"[ID::%2lld][ChangeState] [OLD::%ws] -> [NEW::%ws]\n", mEnemy.lock()->GetGameObjectID(), ToStringState(inOldType).c_str(), ToStringState(inNewType).c_str());
}

const std::wstring StateManager::ToStringState(const EStateType& type)
{
	std::wstring stateStr;
	switch (type)
	{
	case EStateType::State_Unspecified:
		stateStr = L"Unspecified";
		break;
	case EStateType::State_Idle:
		stateStr = L"Idle";
		break;
	case EStateType::State_Round:
		stateStr = L"Round";
		break;
	case EStateType::State_Recovery:
		stateStr = L"Recovery";
		break;
	case EStateType::State_Chase:
		stateStr = L"Chase";
		break;
	case EStateType::State_Attack:
		stateStr = L"Attack";
		break;
	case EStateType::State_Hit:
		stateStr = L"Hit";
		break;
	case EStateType::State_Stun:
		stateStr = L"Stun";
		break;
	case EStateType::State_Death:
		stateStr = L"Death";
		break;
	default:
		break;
	}
	return stateStr;
}
