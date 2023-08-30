#pragma once
class EnemyDarkKnight : public EnemyCharacter
{
public:
	EnemyDarkKnight();
	virtual ~EnemyDarkKnight();

public:
	virtual void OnInitialization() override;

public:
	virtual void OnPatternShot(ActorPtr inVictim) override;

public:
	void RunningAttack();
	void ChargedComboAttack();
	void UppercutAttack();
	void SwingAttack();
	void SwingAndSlamAttack();
	void HandAndSwordSwipeAttack();
	void Berserk();

private:
	PatternInfos<EnemyDarkKnight>		mPatternInfos;
	std::shared_ptr<EnemyMeleeAttack>	mMeleeAttack;
};

