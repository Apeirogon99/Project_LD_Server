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
	virtual bool IsValid()							override;

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) override;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) override;
	
public:
	//void Hit();
	virtual void OnHit(const float inDamage, const bool inIsStun) abstract;
	bool IsAttackRange();

public:
	void SetEnemeyID(const int32 inEnemyID);
	void SetSpawnObjectID(const int64 inSpawnObjectID);
	void SetSpawnLocation(const Protocol::SVector& inSpawnLocation);
	void SetAggroPlayer(const CharacterPtr& inCharacter);

public:
	EnemyCharacterRef			GetEnemyCharacterRef()	{ return std::static_pointer_cast<EnemyCharacter>(shared_from_this()); }
	const int32					GetEnemyID()			{ return mEnemyID; }
	const int64					GetSpawnObjectID()		{ return mSpawnObjectID; }
	StateManager&				GetStateManager()		{ return mStateManager; }
	const Protocol::SVector&	GetSpawnLocation()		{ return mSpawnLocation; }
	const CharacterPtr&			GetAggroPlayer()		{ return mAggroPlayer; }
	const Protocol::SEnemy		ConvertSEnemy();

protected:
	int32						mEnemyID;
	int64						mSpawnObjectID;
	StateManager				mStateManager;
	Protocol::SVector			mSpawnLocation;
	CharacterPtr				mAggroPlayer;
};

