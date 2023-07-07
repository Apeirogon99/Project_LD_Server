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
	
	virtual void SyncStatus(const int64 inDeltaTime) abstract;

	virtual void OnHit(ActorPtr inInstigated, const float inDamage, const Location inHitLocation) override;
	virtual void OnDeath() override;

	virtual void OnAutoAttackShot()			override;
	virtual void OnAutoAttackTargeting()	override;
	virtual void OnAutoAttackOver()			override;
	
public:
	void OnMovementEnemy();
	
public:
	void SetEnemeyID(const int32 inEnemyID);
	void SetSpawnObjectID(const int64 inSpawnObjectID);
	void SetRecoveryLocation(const Location& inRecoveryLocation);
	void SetEnemyStats(const Stats& inEnemyStats);
	void SetAggroPlayer(PlayerCharacterPtr inCharacter);

public:
	EnemyCharacterRef			GetEnemyCharacterRef()		{ return std::static_pointer_cast<EnemyCharacter>(shared_from_this()); }
	const int32					GetEnemyID()				{ return mEnemyID; }
	const int64					GetSpawnObjectID()			{ return mSpawnObjectID; }
	StatsComponent&				GetEnemyStatsComponent()	{ return mStatsComponent; }
	AttackComponent&			GetAttackComponent()		{ return mAttackComponent; }
	StateManager&				GetStateManager()			{ return mStateManager; }
	const Location&				GetRecoveryLocation()		{ return mRecoveryLocation; }
	const PlayerCharacterPtr&	GetAggroPlayer()			{ return mAggroPlayer; }
	const Protocol::SEnemy		ConvertSEnemy();

protected:
	int32						mEnemyID;
	int64						mSpawnObjectID;
	int64						mSyncStatusTime;
	StatsComponent				mStatsComponent;
	AttackComponent				mAttackComponent;

	StateManager				mStateManager;

	Location					mRecoveryLocation;
	PlayerCharacterPtr			mAggroPlayer;
};

