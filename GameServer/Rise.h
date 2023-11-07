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
	virtual bool OnParrying(ActorPtr inActor) override;

public:
	void SetSpawnInfo(EnemyID inEnemyID, int32 inSpawnCount, int32 inSpawnLoop, bool inIsReward, float inSpawnRadius, float inMaxRange, int32 inSKillID);
	void SpawnEnemy();

public:
	SphereCollisionComponent* GetSphereCollisionComponent();

private:
	int64			mStartTime;
	int64			mEndTime;

	EnemyID			mEnemyID;
	int32			mSpawnCount;
	int32			mSpawnLoop;
	float			mSpawnRadius;
	float			mMaxRange;
	int32			mSkillID;

	bool			mIsReward;
};

