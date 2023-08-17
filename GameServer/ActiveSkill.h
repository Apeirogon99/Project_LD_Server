#pragma once
class ActiveSkill : public Actor
{
public:
	ActiveSkill(const WCHAR* inName);
	virtual ~ActiveSkill();

private:
	ActiveSkill(ActiveSkill&&) = delete;
	ActiveSkill& operator=(ActiveSkill&&) = delete;

public:
	virtual void OnInitialization()				 abstract;
	virtual void OnDestroy()					 abstract;
	virtual void OnTick(const int64 inDeltaTime) abstract;
	virtual bool IsValid()						 abstract;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	void SetActiveSkill(const int32& inSkillID, const int64& inActiveTime);

	virtual void Active();
	void DeActive(const int64& inDuration);

public:
	const int32& GetSkillID() const;

protected:
	int32 mSKillID;
	int64 mActiveTime;
};

