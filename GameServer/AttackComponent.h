#pragma once

enum class EAutoAttackType
{
	Attack_None,
	Attack_Melee,
	Attack_Ranged,
};

class AttackComponent
{
public:
	AttackComponent();
	~AttackComponent();

	AttackComponent(const AttackComponent&) = delete;
	AttackComponent(AttackComponent&&) noexcept = delete;

	AttackComponent& operator=(const AttackComponent&) = delete;
	AttackComponent& operator=(AttackComponent&&) noexcept = delete;

public:
	void InitAutoAttack(const EAutoAttackType inAutoAttackType);
	void Update(ActorPtr inInstigated);

public:
	bool DoAutoAttack(ActorPtr inInstigated, ActorPtr inVictim, const float inDamage, const float inRange, const int64 inTargetingTime, const int64 inAutoAttackOverTime);
	//bool DoAutoAttack(ActorPtr inInstigated, ActorPtr inVictim, const float inDamage, const float inRange, const int64 inAutoAttackOverTime);
	void OnAutoAttackTargeting(ActorPtr inInstigated);
	void OnAutoAttackOver(ActorPtr inInstigated);

public:
	ActorRef GetVictim() const;

public:
	bool IsAutoAttacking(ActorPtr inInstigated);
	bool IsAutoAttackRange(ActorPtr inInstigated, const float inRange);

protected:
	EAutoAttackType mAutoAttackType;

	bool			mIsAutoAttack;
	int64			mAutoAttackLastTime;
	int64			mAutoAttackOverTime;

	ActorRef		mVictim;
};