#pragma once

class EnemyRich : public EnemyCharacter
{
public:
	EnemyRich(const WCHAR* inName);
	virtual ~EnemyRich();

public:
	virtual void OnInitialization() abstract;

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

public:
	void Skill_RiseSkeleton();
	void Skill_BlinkAttack();
	void Skill_Explosion();
	void Skill_MultiCasting();
	void Skill_SourSpear(const Location inLocation, const Rotation inRotation);

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

public:
	void RiseDarkKnight();
	void BlinkSturn();
	void SoulSpark();
	void SoulShackles();

private:
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

public:
	void RiseSkeleton();
	void RealmOfDeath();
	void OnslaughtOfShadows();
	void LifeVessel();

private:
	PatternInfos<EnemyRichPhase3> mPatternInfos;
};
