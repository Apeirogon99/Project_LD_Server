#pragma once
class SourSpear : public EnemyAttack
{
public:
	SourSpear();
	virtual ~SourSpear();

	SourSpear(const SourSpear&) = delete;
	SourSpear(SourSpear&&) noexcept = delete;

	SourSpear& operator=(const SourSpear&) = delete;
	SourSpear& operator=(SourSpear&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	virtual void CheackCollision() override;
	virtual void OnParrying(ActorPtr inActor) override;
	virtual void OnMovement() override;

public:
	void Active();
	void SyncLocation(const int64 inDeltaTime);

public:
	ProjectileComponent&		GetProjectileComponent();
	SphereCollisionComponent*	GetSphereCollisionComponent();

private:
	bool mIsCollision;
	bool mIsActive;
	int64 mStartTime;
	ProjectileComponent			mProjectileComponent;
};

