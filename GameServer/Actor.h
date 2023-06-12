#pragma once
class Actor : public GameObject
{
public:
	Actor(const WCHAR* inActorName);
	virtual ~Actor();

public:
	virtual void Initialization()	override;
	virtual void Destroy()			override;
	virtual void Tick()				override;
	virtual bool IsValid()			override;

	void		 CloseToPlayer(PlayerStatePtr inClosePlayerState);
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) abstract;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) abstract;

public:
	void SetOwner(RemotePlayerRef inOwner);

	void SetTransform(const Protocol::SVector& inLocation, const Protocol::SRotator& inRotation, const Protocol::SVector& inScale);
	void SetTransform(const Protocol::STransform& inTransform);

	void SetLocation(const float inX, const float inY, const float inZ);
	void SetLocation(const Protocol::SVector& inLocation);

	void SetRotation(const float inX, const float inY, const float inZ);
	void SetRotation(const Protocol::SRotator& inRotator);

	void SetScale(const float inX, const float inY, const float inZ);
	void SetScale(const Protocol::SVector& inScale);

	void SetVelocity(const float inX, const float inY, const float inZ);
	void SetVelocity(const Protocol::SVector& inVelocity);

public:
	RemotePlayerRef				GetOwner()		const { return mOwner; }
	const Protocol::STransform& GetTransform()	const { return mTransfrom; }
	const Protocol::SVector&	GetLocation()	const { return mTransfrom.location(); }
	const Protocol::SRotator&	GetRotation()	const { return mTransfrom.rotation(); }
	const Protocol::SVector&	GetScale()		const { return mTransfrom.scale(); }
	const Protocol::SVector&	GetVelocity()	const { return mVelocity; }

protected:
	RemotePlayerRef			mOwner;
	Protocol::STransform	mTransfrom;
	Protocol::SVector		mVelocity;
};

