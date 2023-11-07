#pragma once
class Explosion : public EnemyAttack
{
public:
	Explosion();
	virtual ~Explosion();

	Explosion(const Explosion&) = delete;
	Explosion(Explosion&&) noexcept = delete;

	Explosion& operator=(const Explosion&) = delete;
	Explosion& operator=(Explosion&&) noexcept = delete;

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
	virtual bool OnParrying(ActorPtr inActor) override;

public:
	SphereCollisionComponent* GetSphereCollisionComponent();

private:
	int64 mStartTime;
	int64 mEndTime;
};

