#pragma once

class Pawn : public Actor
{
public:
	Pawn(const WCHAR* inName);
	virtual ~Pawn();

public:
	virtual void Initialization()	override;
	virtual void Destroy()			override;
	virtual void Tick()				override;
	virtual bool IsValid()			override;

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) abstract;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) abstract;

public:
	virtual void MoveDestination(Protocol::C2S_MovementCharacter inPakcet) {};
	//virtual void PlayAnimation() abstract;

public:
	void SetMoveLocation(const Protocol::SVector& inMoveLocation);
	void SetMoveLastTick(const int64 inMoveLastTick);
	void SetAnimationType(const int32 inAnimationType);
	void SetAnimationLastTick(const int64 inAnimationLastTick);

public:
	const Protocol::SVector&	GetMoveLocation()		{ return mMoveLocation; }
	const int64					GetMoveLastTick()		{ return mMoveLastTick; }
	const int32					GetAnimationType()		{ return mAnimationType; }
	const int64					GetAnimationLastTick()	{ return mAnimationLastTick; }

protected:
	Protocol::SVector	mMoveLocation;
	int64				mMoveLastTick;

	int32				mAnimationType;
	int64				mAnimationLastTick;
};

