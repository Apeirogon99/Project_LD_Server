#pragma once
class WarriorBuff : public Actor
{
public:
	WarriorBuff();
	virtual ~WarriorBuff();

private:
	WarriorBuff(WarriorBuff&&) = delete;
	WarriorBuff& operator=(WarriorBuff&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid()						 override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	void CheackCollision();

public:
	const int64& GetDuration();

private:
	SphereCollisionComponent	mSphereCollisionComponent;
	StatsComponent				mStatsComponent;

	int64						mDuration;
	std::map<int64, bool>		mOverlapPlayer;
};

