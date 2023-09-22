#pragma once
class SoulSpark : public EnemyAttack
{
public:
	SoulSpark();
	virtual ~SoulSpark();

	SoulSpark(const SoulSpark&) = delete;
	SoulSpark(SoulSpark&&) noexcept = delete;

	SoulSpark& operator=(const SoulSpark&) = delete;
	SoulSpark& operator=(SoulSpark&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	void CheackTargeting();
	virtual void CheackCollision() override;
	virtual void OnParrying(ActorPtr inActor) override;

public:
	bool SearchTarget();

public:
	BoxCollisionComponent* GetBoxCollisionComponent();

private:
	int64 mStartTime;
	int64 mEndTime;

	FVector mTargetLocation;
	FRotator mTargetRotation;

	ActorPtr mTarget;
};
