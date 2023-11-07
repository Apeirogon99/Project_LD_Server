#pragma once
class RealmOfDeath : public EnemyAttack
{
public:
	RealmOfDeath();
	virtual ~RealmOfDeath();

	RealmOfDeath(const RealmOfDeath&) = delete;
	RealmOfDeath(RealmOfDeath&&) noexcept = delete;

	RealmOfDeath& operator=(const RealmOfDeath&) = delete;
	RealmOfDeath& operator=(RealmOfDeath&&) noexcept = delete;

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
	virtual bool OnParrying(ActorPtr inActor) override;

public:
	SphereCollisionComponent* GetSphereCollisionComponent();

private:
	StatsComponent	mStatsComponent;
	int64			mStartTime;
	int64			mEndTime;

	bool			mActive;
	ActorPtr		mTarget;
	Location		mTargetLocation;

	std::map<int64, float> mOverlapPlayer;
};


