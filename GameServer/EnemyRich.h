#pragma once

class EnemyRich : public EnemyCharacter
{
public:
	EnemyRich(const WCHAR* inName);
	virtual ~EnemyRich();

public:
	virtual void OnInitialization() abstract;
	virtual void OnTick(const int64 inDeltaTime) override;
	void DoMoveLocation(FVector inStartLocation, FVector inEndLocation, int64 inDuration, float inSpeed);
	void DoTeleportLocation(FVector inLocation);
	void MoveDone();

protected:
	bool mMove;
	FVector mTempStage;
	float mTempStageLenght;
};

class EnemyRichPhase1 : public EnemyRich
{
public:
	EnemyRichPhase1();
	virtual ~EnemyRichPhase1();

public:
	virtual void OnInitialization() override;

public:
	virtual void OnPatternShot(ActorPtr inVictim) override;
	virtual void OnPatternOver() override;
	virtual void OnReward() override;

public:
	void Skill_RiseSkeleton();
	void Skill_BlinkAttack();
	void Skill_Explosion();
	void Skill_MultiCasting();
	void Skill_SourSpear(ActorPtr inPlayer, Location inLocation);

private:
	PatternInfos<EnemyRichPhase1> mPatternInfos;
};

class EnemyRichPhase2 : public EnemyRich
{
public:
	EnemyRichPhase2();
	virtual ~EnemyRichPhase2();

public:
	virtual void OnInitialization() override;

public:
	virtual void OnPatternShot(ActorPtr inVictim) override;
	virtual void OnPatternOver() override;
	virtual void OnReward() override;

public:
	void Skill_RiseDarkKnight();
	void Skill_BlinkSturn();
	void Skill_SoulSpark();
	void Skill_SoulShackles();

private:
	bool mSpawnDarkKnight;
	PatternInfos<EnemyRichPhase2> mPatternInfos;
};

class EnemyRichPhase3 : public EnemyRich
{
public:
	EnemyRichPhase3();
	virtual ~EnemyRichPhase3();

public:
	virtual void OnInitialization() override;

public:
	virtual void OnPatternShot(ActorPtr inVictim) override;
	virtual void OnPatternOver() override;
	virtual void OnReward() override;

public:
	void Skill_RiseDarkSkeleton();
	void Skill_RealmOfDeath();
	void Skill_OnslaughtOfShadows();
	void Skill_LifeVessel();

private:
	bool mSpawnLifeVessel;
	PatternInfos<EnemyRichPhase3> mPatternInfos;
};

