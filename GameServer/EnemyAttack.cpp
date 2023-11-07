#include "pch.h"
#include "EnemyAttack.h"

EnemyAttack::EnemyAttack(const WCHAR* inName) : Actor(inName), mEnemyAttackType(EEnemyAttackType::Enemy_Attack_Unspecified), mTargetActorType(EActorType::Unspecified), mDamage(0.0f), mCancel(false), mMaxLifeTime(INFINITE), mCurrentLifeTime(0), mIsParrying(false), mParryingStart(0), mParryingEnd(0)
{
	this->SetActorType(static_cast<uint8>(EActorType::EnemyAttack));
}

EnemyAttack::~EnemyAttack()
{

}

void EnemyAttack::ReserveDestroy(const int64& inDelay)
{
	mMaxLifeTime = inDelay;
	mCurrentLifeTime = 0;
}

void EnemyAttack::PushReserveDestroy()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	std::shared_ptr<EnemyCharacter> enemy = std::static_pointer_cast<EnemyCharacter>(owner);
	if (nullptr == enemy)
	{
		return;
	}

	if (enemy->GetAutoAttackComponent().GetAttackType() == EAutoAttackType::Attack_Pattern)
	{
		enemy->PushTask(worldTime, &EnemyCharacter::OnPatternOver);
	}
	else
	{
		enemy->PushTask(worldTime, &EnemyCharacter::OnAutoAttackOver);
	}

	bool ret = world->DestroyActor(this->GetGameObjectID());
	if (false == ret)
	{
		this->GameObjectLog(L"Can't destroy enemy attack\n");
	}
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

void EnemyAttack::SetCancel(const bool& inCancel)
{
	mCancel = inCancel;
}

void EnemyAttack::SetParryinglTime(const int64& inStart, const int64& inEnd)
{
	mParryingStart = inStart;
	mParryingEnd = inEnd;
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

const bool& EnemyAttack::GetCancel() const
{
	return mCancel;
}

const int64& EnemyAttack::GetMaxLifeTime() const
{
	return mMaxLifeTime;
}

const bool EnemyAttack::CanParrying() const
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}
	const int64& worldTime = world->GetWorldTime();

	return mParryingStart <= worldTime && worldTime <= mParryingEnd;
}

const bool EnemyAttack::IsLife(const int64& inDeltaTime)
{

	mCurrentLifeTime += inDeltaTime;
	if (mCurrentLifeTime <= mMaxLifeTime)
	{
		return false;
	}

	return true;
}
