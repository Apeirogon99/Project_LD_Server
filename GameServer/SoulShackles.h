#pragma once
class SoulShackles : public EnemyAttack
{
public:
	SoulShackles();
	virtual ~SoulShackles();

	SoulShackles(const SoulShackles&) = delete;
	SoulShackles(SoulShackles&&) noexcept = delete;

	SoulShackles& operator=(const SoulShackles&) = delete;
	SoulShackles& operator=(SoulShackles&&) noexcept = delete;

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
	SphereCollisionComponent* GetSphereCollisionComponent();

private:
	StatsComponent	mStatsComponent;
	int64			mStartTime;
	int64			mEndTime;

	std::map<int64, bool> mOverlapPlayer;
};