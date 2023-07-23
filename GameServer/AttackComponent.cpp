#include "pch.h"
#include "AttackComponent.h"

AttackComponent::AttackComponent() : mAutoAttackType(EAutoAttackType::Attack_None), mCurrentAutoAttackCount(0), mIsAutoAttack(false), mIsComboAutoAttack(false), mLastAutoAttackTime(0)
{
}

AttackComponent::~AttackComponent()
{

}

void AttackComponent::InitAutoAttack(const EAutoAttackType inAutoAttackType, const AttackInfos& inAttackInfos)
{
	mAutoAttackType		= inAutoAttackType;
	mAttackInfos		= inAttackInfos;
	mIsAutoAttack		= false;
	mLastAutoAttackTime = 0;

	assert(mAttackInfos.size() != 0);
}

bool AttackComponent::Update(ActorPtr inInstigated)
{
	ActorPtr victim = this->mVictimActor.lock();
	if (nullptr == victim)
	{
		return false;
	}

	return true;
}

void AttackComponent::OnOverAutoAttack()
{
	mVictimActor.reset();
	mIsAutoAttack = false;
}

void AttackComponent::OnOverAutoComboAttack()
{
	mVictimActor.reset();
	mIsAutoAttack = false;
	mIsComboAutoAttack = false;
	mCurrentAutoAttackCount = 0;
}

bool AttackComponent::DoMeleeAutoAttack(ActorPtr inInstigated, ActorPtr inVictim, const float& inDamage)
{
	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}
	const int64 worldTime = world->GetWorldTime();
	mLastAutoAttackTime = worldTime;

	if (EAutoAttackType::Attack_Melee != mAutoAttackType)
	{
		return false;
	}

	if (false == IsAutoAttacking(inInstigated))
	{
		return false;
	}

	AttackInfo attackInfo = mAttackInfos.at(0);
	inInstigated->PushTask(mLastAutoAttackTime + attackInfo.GetTargetingTime()	, &Actor::OnAutoAttackTargeting, inDamage, attackInfo.GetAttackRange());
	inInstigated->PushTask(mLastAutoAttackTime + attackInfo.GetOverTime()		, &Actor::OnAutoAttackOver);

	mVictimActor = inVictim;
	mIsAutoAttack = true;
	return true;
}

bool AttackComponent::DoComboMeleeAutoAttack(ActorPtr inInstigated, ActorPtr inVictim, const float& inDamage)
{
	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}
	const int64 worldTime = world->GetWorldTime();
	mLastAutoAttackTime = worldTime;

	if (EAutoAttackType::Attack_Combo_Melee != mAutoAttackType)
	{
		return false;
	}

	AttackInfo attackInfo = mAttackInfos.at(mCurrentAutoAttackCount);
	printf("COMBO ATTACK : [%d] [%lld] [%lld] [%lld]\n", mCurrentAutoAttackCount, attackInfo.GetShotTime(), attackInfo.GetTargetingTime(), attackInfo.GetOverTime());

	if (mCurrentAutoAttackCount == mAttackInfos.size() - 1)
	{
		inInstigated->PushTask(mLastAutoAttackTime + attackInfo.GetOverTime(), &Actor::OnAutoAttackOver);
	}
	else
	{
		inInstigated->PushTask(mLastAutoAttackTime + attackInfo.GetShotTime(), &Actor::OnAutoAttackShot, inVictim);
	}

	inInstigated->PushTask(mLastAutoAttackTime + attackInfo.GetTargetingTime(), &Actor::OnAutoAttackTargeting, inDamage, attackInfo.GetAttackRange());

	mCurrentAutoAttackCount += 1;
	mCurrentAutoAttackCount %= mAttackInfos.size();

	mVictimActor = inVictim;
	mIsAutoAttack = true;
	mIsComboAutoAttack = false;
	return true;
}

bool AttackComponent::IsAutoAttacking(ActorPtr inInstigated)
{
	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		assert(world);
	}

	if (false == mIsAutoAttack)
	{
		return true;
	}

	const int64 worldTime		= world->GetWorldTime();
	const int64 afterAttackTime = worldTime - mLastAutoAttackTime;
	const int64 attackOverTimne = mAttackInfos.at(mCurrentAutoAttackCount).GetOverTime();
	if (afterAttackTime <= attackOverTimne)
	{
		return false;
	}

	mIsAutoAttack = false;
	return true;
}

bool AttackComponent::IsComboAutoAttacking(ActorPtr inInstigated)
{
	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		assert(world);
	}
	const int64 worldTime = world->GetWorldTime();
	const int64 afterAttackTime = worldTime - mLastAutoAttackTime;

	if (false == this->mIsAutoAttack)
	{
		return true;
	}

	if (mCurrentAutoAttackCount != mAttackInfos.size() - 1)
	{
		const int64 attackShotTimne = mAttackInfos.at(mCurrentAutoAttackCount - 1).GetShotTime();
		if (afterAttackTime <= attackShotTimne)
		{
			mIsComboAutoAttack = true;
			return false;
		}
	}

	mIsAutoAttack = false;
	mIsComboAutoAttack = false;
	return false;
}

bool AttackComponent::IsComboShotAutoAttack(ActorPtr inInstigated)
{
	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}
	const int64 worldTime = world->GetWorldTime();

	if (EAutoAttackType::Attack_Combo_Melee != mAutoAttackType)
	{
		return false;
	}

	if (true == mIsComboAutoAttack)
	{
		mIsComboAutoAttack = false;
		return true;
	}

	const int32 currentAttackCount = (mCurrentAutoAttackCount == 0) ? static_cast<int32>(mAttackInfos.size() - 1) : mCurrentAutoAttackCount - 1;
	AttackInfo attackInfo = mAttackInfos.at(currentAttackCount);
	inInstigated->PushTask(mLastAutoAttackTime + attackInfo.GetOverTime(), &Actor::OnAutoAttackOver);

	return false;
}

bool AttackComponent::IsAutoAttackRange(const ActorPtr& inInstigated, const ActorPtr& inVictim, const float& inAttackRange)
{
	
	if (nullptr == inInstigated || nullptr == inVictim)
	{
		return false;
	}

	if (false == inInstigated->IsValid() || false == inVictim->IsValid())
	{
		return false;
	}

	float distance = FVector::Distance2D(inInstigated->GetLocation(), inVictim->GetLocation());
	if (distance >= inAttackRange)
	{
		return false;
	}

	return true;
}

const AttackInfo& AttackComponent::GetAttackInfo(const int32& inIndex)
{
	assert(0 <= inIndex && inIndex < mAttackInfos.size());
	return mAttackInfos.at(inIndex);
}

const int32 AttackComponent::GetAutoAttackCount() const
{
	return mCurrentAutoAttackCount;
}

const bool AttackComponent::IsComboAutoAttack() const
{
	return mIsComboAutoAttack;
}
