#include "pch.h"
#include "AttackComponent.h"

AttackComponent::AttackComponent() : mIsAutoAttack(false), mAutoAttackLastTime(0), mAutoAttackOverTime(0), mAutoAttackType(EAutoAttackType::Attack_None)
{
}

AttackComponent::~AttackComponent()
{

}

void AttackComponent::InitAutoAttack(const EAutoAttackType inAutoAttackType)
{
	mAutoAttackType = inAutoAttackType;
}

void AttackComponent::Update(ActorPtr inInstigated)
{

	//if (false == this->IsAutoAttackRange(inInstigated, 0.0f))
	//{
	//	return;
	//}
	//
	//DoAutoAttack(inInstigated, 0.0f, 0.0f, 0, 0);
}

bool AttackComponent::DoAutoAttack(ActorPtr inInstigated, const float inDamage, const float inRange, const int64 inTargetingTime, const int64 inAutoAttackOverTime)
{
	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}
	const int64 worldTime = world->GetWorldTime();

	//if (EAutoAttackType::Attack_Melee != mAutoAttackType || EAutoAttackType::Attack_Combo_Melee != mAutoAttackType)
	//{
	//	return false;
	//}

	if (false == IsAutoAttacking(inInstigated))
	{
		return false;
	}

	mAutoAttackLastTime = worldTime;
	mAutoAttackOverTime = inAutoAttackOverTime;

	inInstigated->PushTask(worldTime, &Actor::OnAutoAttackShot);

	inInstigated->PushTask(worldTime + inTargetingTime, &Actor::OnAutoAttackTargeting);

	inInstigated->PushTask(worldTime + inAutoAttackOverTime, &Actor::OnAutoAttackOver);
	
	mIsAutoAttack = true;
	return true;
}

bool AttackComponent::IsAutoAttacking(ActorPtr inInstigated)
{
	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}

	if (false == mIsAutoAttack)
	{
		return true;
	}

	const int64 worldTime		= world->GetWorldTime();
	const int64 afterAttackTime = worldTime - mAutoAttackLastTime;
	if (afterAttackTime < mAutoAttackOverTime)
	{
		mIsAutoAttack = false;
		return false;
	}

	mIsAutoAttack = true;
	return true;
}

bool AttackComponent::IsAutoAttackRange(ActorPtr inInstigated, const float inRange)
{
	//float distance = FVector::Distance2D(inInstigated->GetLocation(), mVictim.lock()->GetLocation());
	//if (distance <= inRange)
	//{
	//	return true;
	//}

	//return false;
	return false;
}
