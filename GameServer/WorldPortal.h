#pragma once
class WorldPortal : public SphereTrigger
{
public:
	WorldPortal();
	virtual ~WorldPortal();

private:
	WorldPortal(WorldPortal&&) = delete;
	WorldPortal& operator=(WorldPortal&&) = delete;

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
	void EnterWorldTeleport();
	void EndWorldTeleport();
	void WorldTeleport();

public:
	void SetWaitWorldTeleprotTime(int64 inMaxTeleportTime);
	void SetTeleportWorld(GameWorldRef inGameWorldRef);
	void SetTeleportLevel(const std::string& inLevel);
	void SetTeleportLocation(const FVector& inLocation);
	void BroadCastOverlap(SendBufferPtr inSendBuffer);

private:
	GameWorldRef mTeleportWorld;
	FVector		mTeleportLocation;
	std::string mTeleportLevel;

	int64	mMaxTeleportTime;
	int64	mCurTeleportTime;
	bool	mIsTeleport;
};

