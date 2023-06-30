#pragma once
class Actor : public GameObject
{
public:
	Actor(const WCHAR* inActorName);
	virtual ~Actor();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

	void		 CloseToPlayer(PlayerStatePtr inClosePlayerState);
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) abstract;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) abstract;

public:
	void SetOwner(GameObjectRef inOwner);
	void SetWorld(WorldRef inWorld);

	void SetTransform(const Protocol::SVector& inLocation, const Protocol::SRotator& inRotation, const Protocol::SVector& inScale);
	void SetTransform(const Protocol::STransform& inTransform);
	void SetTransform(const FVector& inLocation, const FRotator& inRotation, const FVector& inScale);

	void SetLocation(const float inX, const float inY, const float inZ);
	void SetLocation(const Protocol::SVector& inLocation);
	void SetLocation(const FVector& inLocation);

	void SetRotation(const float inX, const float inY, const float inZ);
	void SetRotation(const Protocol::SRotator& inRotator);
	void SetRotation(const FRotator& inRotator);

	void SetScale(const float inX, const float inY, const float inZ);
	void SetScale(const Protocol::SVector& inScale);
	void SetScale(const FVector& inScale);

	void SetVelocity(const float inX, const float inY, const float inZ);
	void SetVelocity(const Protocol::SVector& inVelocity);
	void SetVelocity(const FVector& inVelocity);

public:
	ActorPtr					GetActorPtr()			{ return std::static_pointer_cast<Actor>(shared_from_this()); }
	GameObjectRef				GetOwner()		const	{ return mOwner; }
	WorldRef					GetWorld()		const	{ return mWorld; }
	Viewers&					GetViewers()			{ return mViewers; }
	const Protocol::STransform& GetTransform()	const	{ return mTransfrom; }
	const Protocol::SVector&	GetLocation()	const	{ return mTransfrom.location(); }
	const Protocol::SRotator&	GetRotation()	const	{ return mTransfrom.rotation(); }
	const Protocol::SVector&	GetScale()		const	{ return mTransfrom.scale(); }
	const Protocol::SVector&	GetVelocity()	const	{ return mVelocity; }

protected:
	void BrodcastViewers(SendBufferPtr inSendBuffer);

protected:
	GameObjectRef			mOwner;
	WorldRef				mWorld;
	Viewers					mViewers;
	Protocol::STransform	mTransfrom;
	Protocol::SVector		mVelocity;
};

