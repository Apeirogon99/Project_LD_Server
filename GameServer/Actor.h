#pragma once
class Actor
{
public:
	Actor(const char* inClassName, const int64 inGameObjectID);
	Actor(const WCHAR* inActorName, const int64 inGameObjectID);
	virtual ~Actor();

public:
	virtual void Initialization() abstract;
	virtual void Destroy() abstract;
	virtual void Tick() abstract;
	virtual bool IsValid();

public:
	void SetOwner(RemotePlayerRef inOwner);

	void SetLocation(const float inX, const float inY, const float inZ);
	void SetLocation(const Protocol::SVector& inLocation);

	void SetVelocity(const Protocol::SVector& inVelocity);
	void SetRotator(const Protocol::SRotator& inRotator);

public:
	const WCHAR*				GetActorName() const;
	const int64					GetGameObjectID() const;

	RemotePlayerRef				GetOwner() const;
	const Protocol::SVector&	GetLocation() const;
	const Protocol::SVector&	GetVelocity() const;
	const Protocol::SRotator&	GetRotator() const;

protected:
	void SetActorName(const int8* inClassName);

protected:
	const WCHAR*		mActorName;
	int64				mGameObjectID;

	RemotePlayerRef		mOwner;
	Protocol::SVector	mLocation;
	Protocol::SVector	mVelocity;
	Protocol::SRotator	mRotator;
};

