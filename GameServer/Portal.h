#pragma once
class Portal : public SphereTrigger
{
public:
	Portal();
	virtual ~Portal();

private:
	Portal(Portal&&) = delete;
	Portal& operator=(Portal&&) = delete;

public:
	virtual void OnInitialization() override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	virtual void OnBeginOverlap(ActorPtr inBeginOverlapActor) override;
	virtual void OnEndOverlap(ActorPtr inEndOverlapActor) override;
	virtual void OnOverlapTick(const int64 inDeltaTime) override;

public:
	void EnterTeleport();
	void LeaveTeleport();
	
	void BeginTeleport();
	void EndTeleport();

public:
	void SetWaitTeleprotTime(int64 inMaxTeleportTime);
	void SetTeleportLocation(const FVector& inLocation);
	void BroadCastOverlap(SendBufferPtr inSendBuffer);

private:
	FVector mTeleportLocation;

	int64	mMaxTeleportTime;
	int64	mCurTeleportTime;
	int64	mLastTeleportTime;

	bool	mIsTeleport;
};