#pragma once

enum class EDarkKnightAttackType
{
	Running,
	UpperCut,
	LeftToRightSwing,
	RightToLeftSwing,
	Slam,
	Hand,
};

class DarkKnightAttackInfo
{
public:
	DarkKnightAttackInfo();
	DarkKnightAttackInfo(const FVector& inExtent, const float& inDamage, const int64& inParryingTime, const int64& inCheckCollisionTime);
	~DarkKnightAttackInfo();

	FVector mExtent;
	float	mMulDamage;
	int64	mParryingTime;
	int64	mCheckCollisionTime;
};

class EnemyDarkKnight : public EnemyCharacter
{
public:
	EnemyDarkKnight();
	virtual ~EnemyDarkKnight();

public:
	virtual void OnInitialization() override;
	virtual void OnTick(const int64 inDeltaTime) override;

public:
	virtual void OnPatternShot(ActorPtr inVictim) override;
	virtual void OnPatternOver() override;
	virtual void OnReward() override;

	virtual void OnHit(ActorPtr inInstigated, const float inDamage) override;
	virtual void OnDeath() override;

public:
	void BerserkPhase();

public:
	void RunningAttack();
	void ChargedComboAttack();
	void UppercutAttack();
	void SwingAttack();
	void SwingAndSlamAttack();
	void HandAndSwordSwipeAttack();
	void Berserk();

public:
	void DoMeleeAttack(DarkKnightAttackInfo attackInfo);

private:
	PatternInfos<EnemyDarkKnight> mPatternInfos;
	std::map<EDarkKnightAttackType, DarkKnightAttackInfo> mDarkKnightAttacks;

	bool mIsBerserkMode;
};

