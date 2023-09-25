#pragma once

class EnemyCharacter : public Character
{
public:
	EnemyCharacter(const WCHAR* inName);
	virtual ~EnemyCharacter();

public:
	virtual void OnInitialization()					abstract;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

	void OnBeginSpawn();
	void OnEndSpawn();

	virtual void OnSyncLocation(const int64 inDeltaTime);
	virtual void DetectChangeEnemy();

	virtual void OnHit(ActorPtr inInstigated, const float inDamage) override;
	virtual void OnDeath() override;

	virtual void OnReward() abstract;

	virtual void OnBuffChanage(const EStatType inStatType, const float inValue, bool inIsPush);

public:
	virtual void OnPatternShot(ActorPtr inVictim) {}
	//virtual void OnPatternShot(ActorPtr inVictim) {}
	virtual void OnPatternOver() {}

public:
	void OnMovementEnemy();
	
public:
	void SetReward(const bool& inIsReward);
	void SetAggressive(const bool& inIsAggressive);
	void SetEnemeyID(const int32 inEnemyID);
	void SetSpawner(EnemySpawnerRef inSpawner);
	void SetRecoveryLocation(const Location& inRecoveryLocation);
	void SetEnemyStats(const Stats& inEnemyStats);
	void SetAggroActor(ActorRef inCharacter);
	void SetMaxChaseRange(const float& inMaxChaseRange);
	void SetMaxSearchRange(const float& inMaxSearchRange);

	bool IsDeath() const;

public:
	EnemyCharacterRef			GetEnemyCharacterRef()		{ return std::static_pointer_cast<EnemyCharacter>(shared_from_this()); }
	const bool&					GetReward()					{ return mIsReward; }
	const bool&					GetAggressive()				{ return mAggressive; }
	const int32					GetEnemyID()				{ return mEnemyID; }
	EnemySpawnerRef&			GetSpawner()				{ return mSpawner; }
	StatsComponent&				GetEnemyStatsComponent()	{ return mStatsComponent; }
	AttackComponent&			GetAutoAttackComponent()	{ return mAutoAttackComponent; }
	BuffComponent&				GetBuffComponent()			{ return mBuffComponent; }
	StateManager&				GetStateManager()			{ return mStateManager; }
	const Location&				GetRecoveryLocation()		{ return mRecoveryLocation; }
	ActorRef					GetAggroActor()				{ return mAggroActor; }
	float						GetMaxChaseRange()			{ return mMaxChaseRange; }
	float						GetMaxSearchRange()			{ return mMaxSearchRange; }

protected:
	int32						mEnemyID;
	EnemySpawnerRef				mSpawner;
	int32						mSpawnParticle;
	int64						mSpawnParticleDelay;
	StatsComponent				mStatsComponent;
	AttackComponent				mAutoAttackComponent;
	BuffComponent				mBuffComponent;

	bool						mIsSpawn;
	bool						mIsReward;
	bool						mAggressive;
	float						mMaxChaseRange;
	float						mMaxSearchRange;

	StateManager				mStateManager;

	Location					mRecoveryLocation;
	ActorRef					mAggroActor;
};

