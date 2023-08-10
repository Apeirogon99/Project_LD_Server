#include "pch.h"
#include "Skill.h"

Skill::Skill() : GameObject(L"Skill"), mIsLoad(false)
{
}

Skill::~Skill()
{
}

void Skill::OnInitialization()
{
}

void Skill::OnDestroy()
{
}

void Skill::OnTick(const int64 inDeltaTime)
{
}

bool Skill::IsValid()
{
	return false;
}

void Skill::SetLoadSkill(const bool& inLoad)
{
	mIsLoad = inLoad;
}

void Skill::LoadSkillTree(const int32& inSkillID, const int32& inSkillCount)
{
	std::pair<int32, int32> skill = std::make_pair(inSkillID, inSkillCount);
	mSkills.insert(skill);
}

void Skill::UpdateSkillTree(const int32 inSkillID, const int32 inSkillCount)
{

	if (false == CanUpdateSkill(inSkillID))
	{
		return;
	}

	auto find = mSkills.find(inSkillID);
	if (find == mSkills.end())
	{
		std::pair<int32, int32> skill = std::make_pair(inSkillID, 1);
		mSkills.insert(skill);
	}
	else
	{
		find->second += inSkillCount;

		if (find->second <= 0)
		{
			mSkills.erase(inSkillID);
		}

	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr remoteClinet = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClinet)
	{
		return;
	}

	const int64& inCharacterID = remotePlayer->GetToken().GetCharacterID();
	Handle_UpdateSkillTree_Request(remoteClinet, inCharacterID, inSkillID, inSkillCount);
}

void Skill::RollbackSkillTree(const int32& inSkillID, const int32& inSkillCount)
{
	auto find = mSkills.find(inSkillID);
	if (find == mSkills.end())
	{
		std::pair<int32, int32> skill = std::make_pair(inSkillID, 1);
		mSkills.insert(skill);
	}
	else
	{
		find->second -= inSkillCount;

		if (find->second <= 0)
		{
			mSkills.erase(inSkillID);
		}
	}
}

bool Skill::CanUpdateSkill(const int32 inSkillID)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}

	GameDatasPtr data = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == data)
	{
		return false;
	}

	const SkillInfo& skillInfo = data->GetSkillInfo(inSkillID);
	const std::map<int32, int32> skillConditions = skillInfo.GetConditions();
	if(skillConditions.size() == 0)
	{
		return true;
	}

	for (auto conditions : skillConditions)
	{
		auto find = mSkills.find(conditions.first);
		if (find == mSkills.end())
		{
			return false;
		}

		if (conditions.second != find->second && conditions.second > find->second)
		{
			return false;
		}
	}

	return true;
}
