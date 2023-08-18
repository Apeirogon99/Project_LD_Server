#pragma once
class WarriorParrying : public ActiveSkill
{
public:
	WarriorParrying();
	virtual ~WarriorParrying();

private:
	WarriorParrying(WarriorParrying&&) = delete;
	WarriorParrying& operator=(WarriorParrying&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid()						 override;

public:
	virtual void OnHit(ActorPtr inInstigated, const float inDamage) override;

public:
	virtual void Active() override;

public:
	void StartParrying();
	void EndParrying();

public:
	void SetWarriorParrying(const float& inDamage);

private:
	bool mIsParrying;
	bool mIsHit;

	int64 mStartParryingTime;
	int64 mEndParryingTime;
};