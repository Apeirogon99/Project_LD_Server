#include "pch.h"
#include "SkillComponent.h"

SkillComponent::SkillComponent()
{
}

SkillComponent::~SkillComponent()
{
}

void SkillComponent::PushSkill(const int32& inSkillID)
{
	mSkills.insert(std::make_pair(inSkillID, 0));
}

void SkillComponent::ReleaseSkill(const int32& inSkillID)
{
	mSkills.erase(inSkillID);
}

void SkillComponent::UpdateSkillCoolTime(const int64& inDeltaTime)
{
	for (auto useSkill = mUseSkills.begin(); useSkill != mUseSkills.end();)
	{
		auto findSkill = mSkills.find(*useSkill);
		if (findSkill != mSkills.end())
		{
			findSkill->second -= inDeltaTime;
			if (findSkill->second <= 0)
			{
				mUseSkills.erase(useSkill++);
			}
			else
			{
				++useSkill;
			}
		}
	}
}

void SkillComponent::UseSkill(ActiveSkillRef inActiveSkill, const int32& inSkillID, const int64& inDuration)
{
	auto findSkill = mSkills.find(inSkillID);
	if (findSkill == mSkills.end())
	{
		return;
	}

	auto findUseSkill = mUseSkills.find(inSkillID);
	if (findUseSkill != mUseSkills.end())
	{
		return;
	}

	mUseSkills.insert(inSkillID);
	findSkill->second = inDuration;
}

void SkillComponent::SetActiveSkill(ActiveSkillRef inActiveSkill)
{
	mActiveSkill = inActiveSkill;
}

const bool SkillComponent::CanUseSkill(const int32& inSkillID)
{
	auto findSkill = mSkills.find(inSkillID);
	if (findSkill == mSkills.end())
	{
		return false;
	}

	auto findUseSkill = mUseSkills.find(inSkillID);
	if (findUseSkill != mUseSkills.end())
	{
		return false;
	}

	return true;
}

ActiveSkillRef SkillComponent::GetActiveSkill()
{
	return mActiveSkill;
}
