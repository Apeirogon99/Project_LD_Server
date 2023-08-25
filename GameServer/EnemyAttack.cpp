#include "pch.h"
#include "EnemyAttack.h"

EnemyAttack::EnemyAttack(const WCHAR* inName) : Actor(inName), mEnemyAttackType(EEnemyAttackType::Enemy_Attack_Unspecified), mTargetActorType(EActorType::Unspecified), mDamage(0.0f)
{
	this->SetActorType(static_cast<uint8>(EActorType::EnemyAttack));
}

EnemyAttack::~EnemyAttack()
{

}

void EnemyAttack::ReserveDestroy(const int64& inDelay)
{

	if (false == this->IsValid())
	{
		return;
	}

	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	world->PushTask(worldTime + inDelay, &World::DestroyActor, this->GetGameObjectID());
}

void EnemyAttack::SetEnemyAttackType(const EEnemyAttackType& inEnemyAttackType)
{
	mEnemyAttackType = inEnemyAttackType;
}

void EnemyAttack::SetTargetActorType(const EActorType& inTargetActorType)
{
	mTargetActorType = inTargetActorType;
}

void EnemyAttack::SetDamage(const float& inDamage)
{
	mDamage = inDamage;
}

const EEnemyAttackType& EnemyAttack::GetEnemyAttackType() const
{
	return mEnemyAttackType;
}

const EActorType& EnemyAttack::GetTargetActorType() const
{
	return mTargetActorType;
}

const float& EnemyAttack::GetDamage() const
{
	return mDamage;
}
