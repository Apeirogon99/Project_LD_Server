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
	void			AutoAttack(Protocol::C2S_AttackToEnemy pkt);
	virtual void	OnAutoAttackShot(bool inIsRange, ActorPtr inVictim);	//override;
	virtual void	OnAutoAttackTargeting()								{}//override;
	virtual void	OnAutoAttackOver()										{}//override;

public:
	void MovementCharacter(Protocol::C2S_MovementCharacter pkt);

public:
	void SyncLocation(const int64 inDeltaTime);

public:
	void SetCharacterID(const int32& inCharacterID);
	void SetCharacterData(Protocol::SCharacterData inCharacterData);
	//void ReplaceEqipment(const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart);


public:
	int32						GetCharacterID()	{ return mCharacterID; }
	Protocol::SCharacterData&	GetCharacterData()	{ return mCharacterData; }
	StatsComponent&				GetStatComponent() 	{ return mStatComponent; }
	EqipmentComponent&			GetEqipmentStats() 	{ return mEqipmentComponent; }

private:
	bool						mIsLoad;
	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;

	StatsComponent				mStatComponent;
	EqipmentComponent			mEqipmentComponent;
	AttackComponent				mAttackComponent;
};

