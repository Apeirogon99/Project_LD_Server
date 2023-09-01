#pragma once
class Portal : public Actor
{
public:
	Portal();
	virtual ~Portal();

private:
	Portal(Portal&&) = delete;
	Portal& operator=(Portal&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid()						 override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

	virtual void OnInteractive(ActorPtr inActor) override;

public:
	void SetTeleportLocation(const FVector& inLocation);

public:
	BoxCollisionComponent* GetBoxCollisionComponent() const;

private:
	FVector mTeleportLocation;
	int64	mCoolTime;
	bool	mUse;
};

