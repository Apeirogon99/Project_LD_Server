#pragma once
class EnemyArcherSkeleton : public EnemyCharacter
{
public:
	EnemyArcherSkeleton();
	virtual ~EnemyArcherSkeleton();

public:
	virtual void	OnInitialization()					override;

public:
	virtual void	OnAutoAttackShot(ActorPtr inVictim) override;
	virtual void	OnAutoAttackTargeting(const float inDamage, const FVector inRange) override;
	virtual void	OnAutoAttackOver() override;

};