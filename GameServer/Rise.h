#pragma once
class Rise : public EnemyAttack
{
public:
	Rise();
	virtual ~Rise();

	Rise(const Rise&) = delete;
	Rise(Rise&&) noexcept = delete;

	Rise& operator=(const Rise&) = delete;
	Rise& operator=(Rise&&) noexcept = delete;

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

public:
	void SpawnEnemy();

public:
	SphereCollisionComponent* GetSphereCollisionComponent();

private:
	int64			mStartTime;
	int64			mEndTime;
};

