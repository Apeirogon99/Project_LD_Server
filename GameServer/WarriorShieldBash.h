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
	void SetWarriorShieldBash(const float inDamage, const float inDebuffMovement);

public:
	virtual void Active() override;
	void SkillActive();

private:
	float mSturnRadius;
	float mSlowRadius;
	float mDamage;
	float mDebuffMovement;
	int64 mSturnDuration;
	int64 mSlowDuration;
	int64 mActiveTime;
};
