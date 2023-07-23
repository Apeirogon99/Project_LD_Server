#pragma once

enum class EAutoAttackType
{
	Attack_None,
	Attack_Melee,
	Attack_Ranged,
	Attack_Combo_Melee,
	Attack_Combo_Ranged,
};

class AttackInfo
{
public:
	AttackInfo() : mShotTime(0), mTargetingTime(0), mOverTime(0), mAttackRange() {}
	AttackInfo(const int64& inShotTime, const int64& inTargetingTime, const int64& inOverTime, const FVector& inRange) : mShotTime(inShotTime), mTargetingTime(inTargetingTime), mOverTime(inOverTime), mAttackRange(inRange) {}
	~AttackInfo() {}

public:
	void InitAttackInfo(const int64& inShotTime, const int64& inTargetingTime, const int64& inOverTime, const FVector& inRange) { mShotTime = inShotTime; mTargetingTime = inTargetingTime; mOverTime = inOverTime; mAttackRange = inRange; }
	void SetShotTime(const int64& inShotTime) { mShotTime = inShotTime; }
	void SetTargetingTime(const int64& inTargetingTime) { mTargetingTime = inTargetingTime; }
	void SetOverTime(const int64& inOverTime) { mOverTime = inOverTime; }
	void SetAttackRange(const FVector& inRange) { mAttackRange = inRange; }
	
public:
	const int64		GetShotTime()		const { return mShotTime; }
	const int64		GetTargetingTime()	const { return mTargetingTime; }
	const int64		GetOverTime()		const { return mOverTime; }
	const FVector&	GetAttackRange()	const { return mAttackRange; }

private:
	int64	mShotTime;
	int64	mTargetingTime;
	int64	mOverTime;
	FVector mAttackRange;
};

using AttackInfos = std::vector<AttackInfo>;

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
	void InitAutoAttack(const EAutoAttackType inAutoAttackType, const AttackInfos& inAttackInfos);
	bool Update(ActorPtr inInstigated);

	void OnOverAutoAttack();
	void OnOverAutoComboAttack();

public:
	bool DoMeleeAutoAttack(ActorPtr inInstigated, ActorPtr inVictim, const float& inDamage);
	bool DoComboMeleeAutoAttack(ActorPtr inInstigated, ActorPtr inVictim, const float& inDamage);
	//bool DoRangeAutoAttack(ActorPtr inInstigated, ActorPtr inVictim, const float inDamage, const float inRange, const int64 inAutoAttackOverTime);

public:
	bool IsAutoAttacking(ActorPtr inInstigated);
	bool IsComboAutoAttacking(ActorPtr inInstigated);
	bool IsComboShotAutoAttack(ActorPtr inInstigated);
	bool IsAutoAttackRange(const ActorPtr& inInstigated, const ActorPtr& inVictim, const float& inAttackRange);

public:
	const AttackInfo&	GetAttackInfo(const int32& inIndex);
	const int32			GetAutoAttackCount() const;
	const bool			IsComboAutoAttack() const;

protected:
	EAutoAttackType mAutoAttackType;
	AttackInfos		mAttackInfos;

	ActorRef		mVictimActor;

	int32			mCurrentAutoAttackCount;
	bool			mIsAutoAttack;
	bool			mIsComboAutoAttack;
	int64			mLastAutoAttackTime;
};