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
	ActorPtr victim = mVictim.lock();
	if (nullptr == victim)
	{
		return;
	}

	if (false == this->IsAutoAttackRange(inInstigated, 0.0f))
	{
		return;
	}

	DoAutoAttack(inInstigated, victim, 0.0f, 0.0f, 0, 0);
}

bool AttackComponent::DoAutoAttack(ActorPtr inInstigated, ActorPtr inVictim, const float inDamage, const float inRange, const int64 inTargetingTime, const int64 inAutoAttackOverTime)
{
	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}
	const int64 worldTime = world->GetWorldTime();

	if (EAutoAttackType::Attack_Melee != mAutoAttackType)
	{
		return false;
	}

	if (false == IsAutoAttacking(inInstigated))
	{
		return false;
	}

	mVictim = inVictim->GetActorRef();
	if (true == this->IsAutoAttackRange(inInstigated, inRange))
	{

	}

	bool isRange = false;
	float distance = FVector::Distance2D(inInstigated->GetLocation(), inVictim->GetLocation());
	if (distance <= inRange)
	{
		isRange = true;

		mAutoAttackLastTime = worldTime;
		mAutoAttackOverTime = inAutoAttackOverTime;

		//inInstigated->PushTask(worldTime + inTargetingTime, &AttackComponent::OnAutoAttackTargeting, inInstigated, inDamage);

		//inInstigated->PushTask(worldTime + inAutoAttackOverTime, &AttackComponent::OnAutoAttackOver, inInstigated);
	}


	//inInstigated->OnAutoAttackShot(isRange, inVictim);

	return true;
}

void AttackComponent::OnAutoAttackTargeting(ActorPtr inInstigated)
{
	if (false == inInstigated->IsValid())
	{
		return;
	}

	WorldPtr world = inInstigated->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	inInstigated->OnAutoAttackTargeting();
	const int64 worldTime = world->GetWorldTime();

	Location StartLocation = inInstigated->GetLocation();
	Location EndLocation = inInstigated->GetRotation().GetForwardVector() * 32.0f;

	Location hitLocation;
	BoxTrace boxTrace(StartLocation, EndLocation, true, FVector(32.0f, 32.0f, 32.0f), FRotator());

	std::vector<ActorPtr> hitActors;
	for (ActorPtr hitActor : hitActors)
	{
		//hitActor->PushTask(worldTime, &Actor::OnHit, inInstigated, inDamage, hitLocation);
	}
}

void AttackComponent::OnAutoAttackOver(ActorPtr inInstigated)
{
	inInstigated->OnAutoAttackOver();
}

ActorRef AttackComponent::GetVictim() const
{
	return mVictim;
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
		return false;
	}

	return true;
}

bool AttackComponent::IsAutoAttackRange(ActorPtr inInstigated, const float inRange)
{
	float distance = FVector::Distance2D(inInstigated->GetLocation(), mVictim.lock()->GetLocation());
	if (distance <= inRange)
	{
		return true;
	}

	return false;
}
