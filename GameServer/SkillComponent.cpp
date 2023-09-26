#include "pch.h"
#include "SkillComponent.h"

SkillComponent::SkillComponent() : mMaxSyncTime(0), mCurSyncTime(0)
{
}

SkillComponent::~SkillComponent()
{
}

void SkillComponent::Init(GameRemotePlayerRef inOwner, int64 inUpdateTime)
{
	mOwner = inOwner;
	mMaxSyncTime = inUpdateTime;
	mCurSyncTime = 0;
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
	Protocol::S2C_SkillCoolTime skillCoolTimePacket;

	mCurSyncTime += inDeltaTime;
	if (mMaxSyncTime >= mCurSyncTime)
	{
		return;
	}

	for (auto useSkill = mUseSkills.begin(); useSkill != mUseSkills.end();)
	{
		auto findSkill = mSkills.find(*useSkill);
		if (findSkill != mSkills.end())
		{
			findSkill->second -= mCurSyncTime;
			if (findSkill->second <= 0)
			{
				mUseSkills.erase(useSkill++);
			}
			else
			{
				++useSkill;
			}

			skillCoolTimePacket.add_skill_id(findSkill->first);
			skillCoolTimePacket.add_skill_time(findSkill->second);
		}
		else
		{
			++useSkill;
		}
	}

	mCurSyncTime = 0;

	if (skillCoolTimePacket.skill_id_size())
	{
		GameRemotePlayerPtr owner = mOwner.lock();
		if (nullptr == owner)
		{
			return;
		}

		PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(owner->GetRemoteClient().lock());
		if (nullptr == playerState)
		{
			return;
		}

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, skillCoolTimePacket);
		playerState->Send(sendBuffer);
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
