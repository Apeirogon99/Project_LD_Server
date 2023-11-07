#pragma once
class EnemyNomalSkeleton : public EnemyCharacter
{
public:
	EnemyNomalSkeleton();
	virtual ~EnemyNomalSkeleton();

public:
	virtual void	OnInitialization()					override;

public:
	virtual void	OnAutoAttackShot(ActorPtr inVictim) override;
	virtual void	OnAutoAttackTargeting(const float inDamage, const FVector inRange) override;
	virtual void	OnAutoAttackOver() override;

public:
	virtual void OnStunWakeUp() override;
	virtual void OnReward() override;

private:
	std::shared_ptr<EnemyMeleeAttack> mMeleeAttack;
};