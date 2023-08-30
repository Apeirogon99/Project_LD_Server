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

	virtual void OnSyncLocation(const int64 inDeltaTime);
	virtual void DetectChangeEnemy();

	virtual void OnHit(ActorPtr inInstigated, const float inDamage) override;
	virtual void OnDeath() override;

	virtual void OnReward() abstract;

	virtual void OnBuffChanage(const EStatType inStatType, const float inValue, bool inIsPush);

public:
	virtual void OnPatternShot(ActorPtr inVictim) {}
	
public:
	void OnMovementEnemy();
	
public:
	void SetAggressive(const bool& inIsAggressive);
	void SetEnemeyID(const int32 inEnemyID);
	void SetSpawnObjectID(const int64 inSpawnObjectID);
	void SetRecoveryLocation(const Location& inRecoveryLocation);
	void SetEnemyStats(const Stats& inEnemyStats);
	void SetAggroActor(ActorRef inCharacter);

	bool IsDeath() const;

public:
	EnemyCharacterRef			GetEnemyCharacterRef()		{ return std::static_pointer_cast<EnemyCharacter>(shared_from_this()); }
	const bool&					GetAggressive()				{ return mAggressive; }
	const int32					GetEnemyID()				{ return mEnemyID; }
	const int64					GetSpawnObjectID()			{ return mSpawnObjectID; }
	StatsComponent&				GetEnemyStatsComponent()	{ return mStatsComponent; }
	AttackComponent&			GetAutoAttackComponent()	{ return mAutoAttackComponent; }
	BuffComponent&				GetBuffComponent()			{ return mBuffComponent; }
	StateManager&				GetStateManager()			{ return mStateManager; }
	const Location&				GetRecoveryLocation()		{ return mRecoveryLocation; }
	ActorRef					GetAggroActor()				{ return mAggroActor; }

protected:
	int32						mEnemyID;
	int64						mSpawnObjectID;
	StatsComponent				mStatsComponent;
	AttackComponent				mAutoAttackComponent;
	BuffComponent				mBuffComponent;

	bool						mAggressive;

	StateManager				mStateManager;

	Location					mRecoveryLocation;
	ActorRef					mAggroActor;
};

