#pragma once
class AttackTestUnitSpanwer : public Actor
{
public:
	AttackTestUnitSpanwer();
	virtual ~AttackTestUnitSpanwer();

	AttackTestUnitSpanwer(const AttackTestUnitSpanwer&) = delete;
	AttackTestUnitSpanwer(AttackTestUnitSpanwer&&) noexcept = delete;

	AttackTestUnitSpanwer& operator=(const AttackTestUnitSpanwer&) = delete;
	AttackTestUnitSpanwer& operator=(AttackTestUnitSpanwer&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor)			override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor)	override;

public:
	void SendQueue();
	void CreateUnit();

public:
	void MakeMovementPacket(const int64& inObjectID, const FVector& inLocation, const FRotator& inRotation, const int64& inTimeStamp);
	void MakeBeginOverlapPacket(const int64& inObjectID);
	void MakeEndOverlapPacket(const int64& inObjectID);

public:
	float mMaxSpawnRange;
	float mMaxGroundRange;

private:
	int32 mMaxUnit;
	int32 mAddUint;
	int32 mCountUnit;
	
	int64 mCreateSpawnCurrentTime;
	int64 mCreateSpawnCoolTime;

private:
	Protocol::S2C_MovementAttackTestUnit		mMovementPacket;
	Protocol::S2C_BeginOverlapAttackTestUnit	mBeginOverlapPacket;
	Protocol::S2C_EndOverlapAttackTestUnit		mEndOverlapPacket;
};

