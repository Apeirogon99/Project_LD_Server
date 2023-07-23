#pragma once
class EnemySlime : public EnemyCharacter
{
public:
	EnemySlime();
	virtual ~EnemySlime();

public:
	virtual void	OnInitialization()					override;
	virtual void	OnDestroy()							override;
	virtual void	OnTick(const int64 inDeltaTime)		override;
	virtual bool	IsValid()							override;

public:
	virtual void	OnAutoAttackShot(ActorPtr inVictim) override;
	virtual void	OnAutoAttackTargeting(const float inDamage, const FVector inRange) override;
	virtual void	OnAutoAttackOver() override;

};

