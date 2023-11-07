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

class MovePlane
{
public:
	MovePlane(const float& inDistance, const int64& inTime) : mDistance(inDistance), mTime(inTime) {}
	~MovePlane() {}

public:
	float mDistance;
	int64 mTime;
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
	virtual void OnStunWakeUp() override;
	virtual void OnReward() override;

	virtual void OnHit(ActorPtr inInstigated, const float inDamage) override;

public:
	bool BerserkPhase();

public:
	void RunningAttack();
	void ChargedComboAttack();
	void UppercutAttack();
	void SwingAttack();
	void SwingAndSlamAttack();
	void HandAndSwordSwipeAttack();

public:
	void DoMeleeHardAttack(DarkKnightAttackInfo inAttackInfo, Rotation inRotation);
	void DoMeleeNomalAttack(DarkKnightAttackInfo inAttackInfo, Rotation inRotation, int64 inDestroyTime);

	void MakeMovePlane(const int64& inWorldTime, std::vector<MovePlane> inMovePlanes);
	void DoMoveLocation(FVector inStartLocation, FVector inEndLocation, int64 inDuration);

private:
	PatternInfos<EnemyDarkKnight> mPatternInfos;
	std::map<EDarkKnightAttackType, DarkKnightAttackInfo> mDarkKnightAttacks;

	bool mIsBerserkMode;
};

