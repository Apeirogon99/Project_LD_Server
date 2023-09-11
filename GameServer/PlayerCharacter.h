#pragma once
class PlayerCharacter : public Character
{
public:
	PlayerCharacter();
	virtual ~PlayerCharacter();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void			SetLoadCharacter(bool inIsLoad);
	virtual void	OnAppearActor(ActorPtr inAppearActor) override;
	virtual void	OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	void			AutoAttack(const int64 inAttackingObjectID);
	void			DoAutoAttack(ActorPtr inVictimActor);
	virtual void	OnHit(ActorPtr inInstigated, const float inDamage) override;
	virtual void	OnAutoAttackShot(ActorPtr inVictim) override;
	virtual void	OnAutoAttackTargeting(const float inDamage, const FVector inRange) override;
	virtual void	OnAutoAttackOver() override;

public:
	void MovementCharacter(Protocol::C2S_MovementCharacter pkt);
	void OnMovement();

public:
	void NextPlayerMode();
	void SyncLocation(const int64 inDeltaTime);
	void DetectChangePlayer();

public:
	void SetCharacterID(const int32& inCharacterID);
	void SetCharacterData(Protocol::SCharacterData inCharacterData);
	void SetPlayerMode(const EPlayerMode& inPlayerMode);
	void SetTargetActor(ActorRef inTargetActor);

public:
	int32						GetCharacterID()		{ return mCharacterID; }
	Protocol::SCharacterData&	GetCharacterData()		{ return mCharacterData; }
	const EPlayerMode&			GetPlayerMode()			{ return mPlayerMode; }
	ActorRef					GetTargetActor()		{ return mTargetActor; }
	StatsComponent&				GetStatComponent() 		{ return mStatComponent; }
	EqipmentComponent&			GetEqipmentComponent() 	{ return mEqipmentComponent; }
	LevelComponent&				GetLevelComponent()		{ return mLevelComponent; }
	SkillComponent&				GetSkillComponent()		{ return mSkillComponent; }
	BuffComponent&				GetBuffComponent()		{ return mBuffComponent; }

private:
	bool						mIsLoad;
	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;

	EPlayerMode					mPlayerMode;
	ActorRef					mTargetActor;

	StatsComponent				mStatComponent;
	EqipmentComponent			mEqipmentComponent;
	AttackComponent				mAutoAttackComponent;
	LevelComponent				mLevelComponent;
	SkillComponent				mSkillComponent;
	BuffComponent				mBuffComponent;
};

