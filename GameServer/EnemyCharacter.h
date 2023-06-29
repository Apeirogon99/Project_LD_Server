#pragma once
class EnemyCharacter : public Pawn
{
public:
	EnemyCharacter(const WCHAR* inName);
	virtual ~EnemyCharacter();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override { return mEnemyID != 0; }

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) abstract;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) abstract;
	
	bool IsAttackRange();

public:
	void SetEnemeyID(const int32 inEnemyID);
	void SetSpawnObjectID(const int64 inSpawnObjectID);
	void SetStats(const Stats& inStats);
	void SetSpawnLocation(const Protocol::SVector& inSpawnLocation);

public:
	const int32					GetEnemyID()		{ return mEnemyID; }
	const int64					GetSpawnObjectID()	{ return mSpawnObjectID; }
	const Stats&				GetStat()			{ return mStats; }
	const StateManager&			GetStateManager()	{ return mStateManager; }
	const Protocol::SVector&	GetSpawnLocation()	{ return mSpawnLocation; }
	const Protocol::SEnemy		ConvertSEnemy();

private:
	int32 mEnemyID;
	int64 mSpawnObjectID;

	Stats mStats;

	StateManager mStateManager;

	Protocol::SVector mSpawnLocation;
};

