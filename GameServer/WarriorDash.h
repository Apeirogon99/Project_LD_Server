#pragma once
class WarriorDash : public ActiveSkill
{
public:
	WarriorDash();
	virtual ~WarriorDash();

private:
	WarriorDash(WarriorDash&&) = delete;
	WarriorDash& operator=(WarriorDash&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid()						 override;

public:
	virtual void Active() override;

public:
	void SetWarriorDash(float inDistance, float inSpeed);

private:
	float mDashDistance;
	float mInitDashSpeed;
	float mDashSpeed;
};

