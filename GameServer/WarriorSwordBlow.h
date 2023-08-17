#pragma once
class WarriorSwordBlow : public ActiveSkill
{
public:
	WarriorSwordBlow();
	virtual ~WarriorSwordBlow();

private:
	WarriorSwordBlow(WarriorSwordBlow&&) = delete;
	WarriorSwordBlow& operator=(WarriorSwordBlow&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid()						 override;

public:
	virtual void Active() override;

public:
	void SetWarriorSwordBlow(const float& inDamage);

public:
	const bool& IsCharge() const;

private:
	bool mIsCharge;
	FVector mChargeVelocity;
	float	mMaxChargeDuration;
};

