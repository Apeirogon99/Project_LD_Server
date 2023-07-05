#pragma once
class EnemyCharacter : public Character
{
public:
	EnemyCharacter(const WCHAR* inName);
	virtual ~EnemyCharacter();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;
	
public:
	
public:
	void SetEnemeyID(const int32 inEnemyID);
	void SetSpawnObjectID(const int64 inSpawnObjectID);
	void SetRecoveryLocation(const Location& inRecoveryLocation);
	void SetEnemyStats(const Stats& inEnemyStats);
	void SetAggroPlayer(const CharacterPtr& inCharacter);

public:
	EnemyCharacterRef			GetEnemyCharacterRef()		{ return std::static_pointer_cast<EnemyCharacter>(shared_from_this()); }
	const int32					GetEnemyID()				{ return mEnemyID; }
	const int64					GetSpawnObjectID()			{ return mSpawnObjectID; }
	const StatsComponent&		GetEnemyStatsComponent()	{ return mStatsComponent; }
	StateManager&				GetStateManager()			{ return mStateManager; }
	const Location&				GetRecoveryLocation()		{ return mRecoveryLocation; }
	const CharacterPtr&			GetAggroPlayer()			{ return mAggroPlayer; }
	const Protocol::SEnemy		ConvertSEnemy();

protected:
	int32						mEnemyID;
	int64						mSpawnObjectID;
	StatsComponent				mStatsComponent;

	StateManager				mStateManager;

	Location					mRecoveryLocation;
	CharacterPtr				mAggroPlayer;
};

