#pragma once
class AttackTestUnit : public Actor
{
public:
	AttackTestUnit();
	virtual ~AttackTestUnit();

	AttackTestUnit(const AttackTestUnit&) = delete;
	AttackTestUnit(AttackTestUnit&&) noexcept = delete;
	
	AttackTestUnit& operator=(const AttackTestUnit&) = delete;
	AttackTestUnit& operator=(AttackTestUnit&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor)			override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor)	override;

	void TickWallCollision();
	void TickUnitCollision();

public:
	ProjectileComponent&	GetProjectileComponent();
	BoxCollisionComponent*	GetBoxCollisionComponent() const;

private:
	bool					mOverlap;
	ProjectileComponent		mProjectileComponent;
	TimeStamp				mTimeStamp;
};