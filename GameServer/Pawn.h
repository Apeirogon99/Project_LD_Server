#pragma once

class Pawn : public Actor
{
public:
	Pawn(const WCHAR* inName);
	virtual ~Pawn();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) abstract;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) abstract;

public:
	void MoveDestination(Protocol::C2S_MovementCharacter inPacket);
	void MovingDestination(const int64 inTime);
	virtual void OnMovement(const int64 inTimeStamp) abstract;
	//virtual void PlayAnimation() abstract;

public:
	void SetMoveLocation(const Protocol::SVector& inMoveLocation);
	void SetMoveLocation(const FVector& inMoveLocation);
	void SetMoveLastTick(const int64 inMoveLastTick);
	void SetAnimationType(const int32 inAnimationType);
	void SetAnimationLastTick(const int64 inAnimationLastTick);
	void SetStats(const Stats& inStats);

public:
	const Protocol::SVector&	GetMoveLocation()		const { return mMoveLocation; }
	const int64					GetMoveLastTick()		const { return mMoveLastTick; }
	const int32					GetAnimationType()		const { return mAnimationType; }
	const int64					GetAnimationLastTick()	const { return mAnimationLastTick; }
	const Stats&				GetStats()				const { return mStats; }

protected:
	Protocol::SVector	mMoveLocation;
	int64				mMoveLastTick;
	int64				mMoveSyncTick;

	int32				mAnimationType;
	int64				mAnimationLastTick;

	Stats				mStats;
};

