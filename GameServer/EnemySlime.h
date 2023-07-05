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
	virtual void	OnHit(ActorPtr inInstigated, const float inDamage, const Location inHitLocation)	{}//override;
	virtual void	OnAutoAttackShot(bool inIsRange, ActorPtr inVictim)									{}//override;
	virtual void	OnAutoAttackTargeting()																{}//override;
	virtual void	OnAutoAttackOver()																	{}//override;

protected:

};

