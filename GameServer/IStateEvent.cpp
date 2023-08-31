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

	if (true == enemy->GetAggressive())
	{
		enemy->GetStateManager().SetState(EStateType::State_Search);
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

	if (true == enemy->GetAggressive())
	{
		enemy->GetStateManager().SetState(EStateType::State_Search);
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
	const float collisionRadius = enemy->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();

	enemy->GetMovementComponent().SetNewDestination(enemy->GetActorPtr(), currentLocation, nextLocation, world->GetWorldTime(), 0.0f);
	enemy->SetRecoveryLocation(nextLocation);
	enemy->OnMovementEnemy();
}

void RoundState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	if (false == enemy->GetMovementComponent().Update(enemy->GetActorPtr(), 5.0f))
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

	if (true == enemy->GetAggressive())
	{
		enemy->GetStateManager().SetState(EStateType::State_Search);
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
	const float collisionRadius = enemy->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();
	this->mCurrentLocation		= enemy->GetLocation();
	this->mMaxDistanceLength	= FVector::Distance2D(this->mCurrentLocation, recoveryLocation);

	enemy->GetMovementComponent().SetNewDestination(enemy->GetActorPtr(), currentLocation, recoveryLocation, world->GetWorldTime(), 0.0f);
	enemy->OnMovementEnemy();
}

void RecoveryState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
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

	//const float fullHealth = enemy->GetEnemyStatsComponent().GetMaxStats().GetHealth();
	//enemy->GetEnemyStatsComponent().UpdateCurrentStat(EStatType::Stat_Health, fullHealth);
}

//==========================//
//		    SEARCH			//
//==========================//

void SearchState::Enter(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}
}

void SearchState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
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
	const int64 worldTime = world->GetWorldTime();

	std::vector<ActorPtr> targetActors;
	bool result = world->FindActors(enemy->GetLocation(), 2000.0f, static_cast<uint8>(EActorType::Player), targetActors, 1);
	if (true == result)
	{
		enemy->SetAggroActor(targetActors.at(0));
		enemy->GetStateManager().SetState(EStateType::State_Chase);
	}

}

void SearchState::Exit(EnemyCharacterRef inEnemy)
{
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
	const Stats& currentStats	= enemy->GetEnemyStatsComponent().GetCurrentStats();
	const float range			= currentStats.GetRange();
	const float velocity		= currentStats.GetMovementSpeed();

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	ActorPtr aggroActor = enemy->GetAggroActor().lock();
	if (nullptr == aggroActor)
	{
		if (true == enemy->GetAggressive())
		{
			enemy->GetStateManager().SetState(EStateType::State_Search);
		}
		else
		{
			enemy->GetStateManager().SetState(EStateType::State_Recovery);
		}
		return;
	}

	Location currentLocation = enemy->GetLocation();
	Location aggroLocation = aggroActor->GetLocation();

	enemy->SetVelocity(velocity, velocity, velocity);
	enemy->GetMovementComponent().SetNewDestination(enemy->GetActorPtr(), currentLocation, aggroLocation, worldTime, range);

	mChaseToRecoveryTime = 0;
}

void ChaseState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}
	const Stats& currentStats = enemy->GetEnemyStatsComponent().GetCurrentStats();
	const float range = currentStats.GetRange();
	const float velocity = currentStats.GetMovementSpeed();

	ActorPtr aggroActor = enemy->GetAggroActor().lock();
	if (nullptr == aggroActor)
	{
		if (true == enemy->GetAggressive())
		{
			enemy->GetStateManager().SetState(EStateType::State_Search);
		}
		else
		{
			enemy->GetStateManager().SetState(EStateType::State_Recovery);
		}
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	//DEBUG
	const float debugDuration = 0.05f;
	PacketUtils::DebugDrawSphere(enemy->GetPlayerViewers(), enemy->GetLocation(), range, debugDuration);


	if (true == enemy->GetAutoAttackComponent().IsAutoAttackRange(enemy->GetActorPtr(), aggroActor->GetActorPtr(), range))
	{
		enemy->GetStateManager().SetState(EStateType::State_Attack);
		return;
	}
	enemy->GetMovementComponent().Update(enemy->GetActorPtr(), 10.0f);

	Location currentLocation = enemy->GetLocation();
	Location aggroLocation = aggroActor->GetLocation();

	enemy->SetVelocity(velocity, velocity, velocity);
	enemy->GetMovementComponent().SetNewDestination(enemy->GetActorPtr(), currentLocation, aggroLocation, worldTime, range);


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
	const float damage		= StatUtils::RandomDamage(stat.GetCurrentStats().GetAttackDamage());


	enemy->SetVelocity(0.0f, 0.0f, 0.0f);

	switch (enemy->GetAutoAttackComponent().GetAttackType())
	{
	case EAutoAttackType::Attack_None:
		break;
	case EAutoAttackType::Attack_Melee:
		enemy->GetAutoAttackComponent().DoMeleeAutoAttack(enemy->GetActorPtr(), aggroActor, damage);
		break;
	case EAutoAttackType::Attack_Ranged:
		enemy->GetAutoAttackComponent().DoRangeAutoAttack(enemy->GetActorPtr(), aggroActor, damage);
		break;
	case EAutoAttackType::Attack_Combo_Melee:
		break;
	case EAutoAttackType::Attack_Combo_Ranged:
		break;
	case EAutoAttackType::Attack_Pattern:
		enemy->OnPatternShot(enemy->GetActorPtr());
		break;
	default:
		break;
	}
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
	mStunToChaseTime = 0;
	mStunTime = 0;
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
	enemy->OnDeath();
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
	mStateTypes.insert(std::make_pair(EStateType::State_Search, static_cast<IStateEvent*>(new SearchState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Chase, static_cast<IStateEvent*>(new ChaseState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Attack, static_cast<IStateEvent*>(new AttackState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Hit, static_cast<IStateEvent*>(new HitState())));
	mStateTypes.insert(std::make_pair(EStateType::State_Stun, static_cast<IStateEvent*>(new StunState())));
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
	EnemyCharacterPtr enemy = mEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	if (mCurrentState == EStateType::State_Unspecified)
	{
		mCurrentState = inStateType;
		EnterState();
		enemy->DetectChangeEnemy();
		return;
	}

	mOldState = mCurrentState;

	IStateEvent* oldState = mStateTypes.at(mCurrentState);
	oldState->Exit(mEnemy);

	mCurrentState = inStateType;

	IStateEvent* newState = mStateTypes.at(mCurrentState);
	newState->Enter(mEnemy);

	enemy->DetectChangeEnemy();
}

const EStateType& StateManager::GetCurrentStateType() const
{
	return mCurrentState;
}

void StateManager::StateChangeDebugPrint()
{
	wprintf(L"[ID::%2lld][ChangeState] [OLD::%ws] -> [NEW::%ws]\n", mEnemy.lock()->GetGameObjectID(), ToStringState(mOldState).c_str(), ToStringState(mCurrentState).c_str());
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

IStateEvent* StateManager::GetCurrentStateEvent()
{
	auto findStateEvent = mStateTypes.find(this->GetCurrentStateType());
	if (findStateEvent == mStateTypes.end())
	{
		return nullptr;
	}

	return findStateEvent->second;
}