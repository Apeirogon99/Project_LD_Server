#pragma once
class WarriorBuff : public ActiveSkill
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
	virtual void Active() override;

private:
	SphereCollisionComponent	mSphereCollisionComponent;
	StatsComponent				mStatsComponent;

	std::map<int64, bool>		mOverlapPlayer;
};

