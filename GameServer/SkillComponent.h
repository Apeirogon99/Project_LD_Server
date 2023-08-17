#pragma once
class SkillComponent
{
public:
	SkillComponent();
	~SkillComponent();

	SkillComponent(const SkillComponent&) = delete;
	SkillComponent(SkillComponent&&) noexcept = delete;

	SkillComponent& operator=(const SkillComponent&) = delete;
	SkillComponent& operator=(SkillComponent&&) noexcept = delete;

public:
	void PushSkill(const int32& inSkillID);
	void ReleaseSkill(const int32& inSkillID);

public:
	void UpdateSkillCoolTime(const int64& inDeltaTime);

public:
	void UseSkill(ActiveSkillRef inActiveSkill, const int32& inSkillID, const int64& inDuration);
	void SetActiveSkill(ActiveSkillRef inActiveSkill);

public:
	const bool CanUseSkill(const int32& inSkillID);
	ActiveSkillRef GetActiveSkill();

private:
	ActiveSkillRef			mActiveSkill;
	std::map<int32, int64>	mSkills;
	std::set<int32>			mUseSkills;
};