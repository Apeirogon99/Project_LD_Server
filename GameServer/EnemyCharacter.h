#pragma once

class EnemyCharacter : public Character
{
public:
	EnemyCharacter(const WCHAR* inName);
	virtual ~EnemyCharacter();

public:
	virtual void OnInitialization()					abstract;
	virtual void OnDestroy()						abstract;
	virtual void OnTick(const int64 inDeltaTime)	abstract;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

	virtual void OnSyncLocation(const int64 inDeltaTime);
	virtual void DetectChangeEnemy();

	virtual void OnHit(ActorPtr inInstigated, const float inDamage) override;
	virtual void OnDeath() override;
	
public:
	void OnMovementEnemy();
	
public:
	void SetEnemeyID(const int32 inEnemyID);
	void SetSpawnObjectID(const int64 inSpawnObjectID);
	void SetRecoveryLocation(const Location& inRecoveryLocation);
	void SetEnemyStats(const Stats& inEnemyStats);
	void SetAggroActor(ActorRef inCharacter);

	bool IsDeath() const;

public:
	EnemyCharacterRef			GetEnemyCharacterRef()		{ return std::static_pointer_cast<EnemyCharacter>(shared_from_this()); }
	const int32					GetEnemyID()				{ return mEnemyID; }
	const int64					GetSpawnObjectID()			{ return mSpawnObjectID; }
	StatsComponent&				GetEnemyStatsComponent()	{ return mStatsComponent; }
	AttackComponent&			GetAutoAttackComponent()	{ return mAutoAttackComponent; }
	StateManager&				GetStateManager()			{ return mStateManager; }
	const Location&				GetRecoveryLocation()		{ return mRecoveryLocation; }
	ActorRef					GetAggroActor()				{ return mAggroActor; }

protected:
	int32						mEnemyID;
	int64						mSpawnObjectID;
	StatsComponent				mStatsComponent;
	AttackComponent				mAutoAttackComponent;

	StateManager				mStateManager;

	Location					mRecoveryLocation;
	ActorRef					mAggroActor;
};

