#include "pch.h"
#include "SkillTree.h"

SkillTree::SkillTree() : GameObject(L"SkillTree"), mIsLoad(false)
{
}

SkillTree::~SkillTree()
{
}

void SkillTree::OnInitialization()
{
}

void SkillTree::OnDestroy()
{
}

void SkillTree::OnTick(const int64 inDeltaTime)
{
}

bool SkillTree::IsValid()
{
	return false;
}

void SkillTree::SetLoadSkillTree(const bool& inLoad)
{
	mIsLoad = inLoad;
}

void SkillTree::LoadSkillTree(const int32& inSkillID, const int32& inSkillCount)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	remotePlayer->GetCharacter()->GetSkillComponent().PushSkill(inSkillID);

	std::pair<int32, int32> skill = std::make_pair(inSkillID, inSkillCount);
	mSkillTrees.insert(skill);
}

void SkillTree::UpdateSkillTree(const int32 inSkillID, const int32 inSkillCount)
{

	if (false == CanUpdateSkill(inSkillID))
	{
		return;
	}

	auto find = mSkillTrees.find(inSkillID);
	if (find == mSkillTrees.end())
	{
		std::pair<int32, int32> skill = std::make_pair(inSkillID, 1);
		mSkillTrees.insert(skill);
	}
	else
	{
		find->second += inSkillCount;

		if (find->second <= 0)
		{
			mSkillTrees.erase(inSkillID);
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

void SkillTree::RollbackSkillTree(const int32& inSkillID, const int32& inSkillCount)
{
	auto find = mSkillTrees.find(inSkillID);
	if (find == mSkillTrees.end())
	{
		std::pair<int32, int32> skill = std::make_pair(inSkillID, 1);
		mSkillTrees.insert(skill);
	}
	else
	{
		find->second -= inSkillCount;

		if (find->second <= 0)
		{
			mSkillTrees.erase(inSkillID);
		}
	}
}

bool SkillTree::CanUpdateSkill(const int32 inSkillID)
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

	const SkillTreeInfo& skillInfo = data->GetSkillInfo(inSkillID);
	const std::map<int32, int32> skillConditions = skillInfo.GetConditions();
	if(skillConditions.size() == 0)
	{
		return true;
	}

	for (auto conditions : skillConditions)
	{
		auto find = mSkillTrees.find(conditions.first);
		if (find == mSkillTrees.end())
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
