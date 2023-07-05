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
	if (mIdleToRoundTime >= IDLE_TO_ROUND_MAX_TIME)
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

	std::shared_ptr<EnemySpawner<EnemySlime>> spawner = std::static_pointer_cast<EnemySpawner<EnemySlime>>(enemy->GetOwner().lock());
	if (nullptr == spawner)
	{
		return;
	}

	WorldPtr world = enemy->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	//const int64 serviceTimeStamp = world->GetServiceTimeStamp();
	//enemy->SetMoveLocation(spawner->GetRandomLocation());
	//enemy->SetMoveLastTick(serviceTimeStamp);
	//enemy->OnMovement(serviceTimeStamp);
}

void RoundState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
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

	//const int64 serviceTimeStamp = world->GetServiceTimeStamp();
	//const float closeToDestination = MAX_LOCATION_DISTANCE;
	//const float locationDistance = FVector::Distance2D(PacketUtils::ToFVector(enemy->GetLocation()), //PacketUtils::ToFVector(enemy->GetMoveLocation()));
	//if (locationDistance > closeToDestination)
	//{
	//	enemy->MovingDestination(serviceTimeStamp);
	//}
	//else
	//{
	//	enemy->GetStateManager().SetState(EStateType::State_Idle);
	//}
}

void RoundState::Exit(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	//enemy->SetMoveLocation(FVector());
	//enemy->SetMoveLastTick(0);
}

//==========================//
//		   RECOVERY			//
//==========================//

void RecoveryState::Enter(EnemyCharacterRef inEnemy)
{
}

void RecoveryState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
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

	//const float closeToDestination = MAX_LOCATION_DISTANCE;
	//const float locationDistance = FVector::Distance2D(PacketUtils::ToFVector(enemy->GetLocation()), //PacketUtils::ToFVector(enemy->GetSpawnLocation()));
	//if (locationDistance > closeToDestination)
	//{
	//	const int64 serviceTimeStamp = world->GetServiceTimeStamp();
	//	enemy->MovingDestination(serviceTimeStamp);
	//}
	//else
	//{
	//	enemy->GetStateManager().SetState(EStateType::State_Idle);
	//}
}

void RecoveryState::Exit(EnemyCharacterRef inEnemy)
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

	mChaseToRecoveryTime = 0;
}

void ChaseState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
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

	//CharacterPtr arroPlayer			= enemy->GetAggroPlayer();
	//const float enemyAttackRange	= enemy->GetBasicStats().GetRange();
	//const float closeToAggroPlayer	= FVector::Distance2D(PacketUtils::ToFVector(enemy->GetLocation()), //PacketUtils::ToFVector(arroPlayer->GetLocation()));
	//if (enemyAttackRange > closeToAggroPlayer)
	//{
	//	enemy->GetStateManager().SetState(EStateType::State_Attack);
	//	return;
	//}
	//
	//const int64 serviceTimeStamp = world->GetServiceTimeStamp();
	//enemy->MovingDestination(serviceTimeStamp);

	mChaseToRecoveryTime += inDeltaTime;
	if (mChaseToRecoveryTime >= CHASE_TO_RECOVERY_MAX_TIME)
	{
		enemy->GetStateManager().SetState(EStateType::State_Recovery);
	}

}

void ChaseState::Exit(EnemyCharacterRef inEnemy)
{
	EnemyCharacterPtr enemy = inEnemy.lock();
	if (nullptr == enemy)
	{
		return;
	}

	enemy->SetAggroPlayer(nullptr);
}

//==========================//
//		    ATTACK			//
//==========================//

void AttackState::Enter(EnemyCharacterRef inEnemy)
{
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
	//TODO : StunTime
	if (mHitToChaseTime >= HIT_TO_CHASE_MAX_TIME)
	{
		enemy->GetStateManager().SetState(EStateType::State_Chase);
	}
}

void HitState::Exit(EnemyCharacterRef inEnemy)
{
}

//==========================//
//		    DEATH			//
//==========================//

void DeathState::Enter(EnemyCharacterRef inEnemy)
{
	//TODO: 아이템 생성
	//막타한테 경험치?
	//Destroy
}

void DeathState::Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)
{
}

void DeathState::Exit(EnemyCharacterRef inEnemy)
{
}