#pragma once
class WarriorShieldBash : public ActiveSkill
{
public:
	WarriorShieldBash();
	virtual ~WarriorShieldBash();

private:
	WarriorShieldBash(WarriorShieldBash&&) = delete;
	WarriorShieldBash& operator=(WarriorShieldBash&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid()						 override;

public:
	void SetWarriorShieldBash(const float inDamage, const float inDebuffMovement, const int64& inSturnDuration, const int64& inSlowDuration, const int64& inActiveSturnTime, const int64& inActiveSlowTime);

public:
	virtual void Active() override;
	void SturnActive();
	void SlowActive();

private:
	float mSturnRadis;
	float mMaxRadius;
	float mDamage;
	float mDebuffMovement;
	int64 mSturnDuration;
	int64 mSlowDuration;
	int64 mActiveSturnTime;
	int64 mActiveSlowTime;
};
