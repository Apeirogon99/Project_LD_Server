#include "pch.h"
#include "BuffComponent.h"

BuffComponent::BuffComponent() : mIsChanage(false)
{
}

BuffComponent::~BuffComponent()
{
}

void BuffComponent::ReapplyBuff(StatsComponent& inStatsComponent)
{
	for (auto buff : mBuffs)
	{
		const float value = inStatsComponent.GetCurrentStats().GetStat(static_cast<int32>(buff.first) - 1);
		inStatsComponent.UpdateCurrentStat(buff.first, value + buff.second);
	}
}

void BuffComponent::PushBuff(StatsComponent& inStatsComponent, const EStatType& inStatType, const float& inValue)
{
	auto findBuff = mBuffs.find(inStatType);
	if (findBuff == mBuffs.end())
	{
		std::pair<EStatType, float> buff = std::make_pair(inStatType, inValue);
		mBuffs.insert(buff);
	}
	else
	{
		findBuff->second += inValue;
	}

	const float value = inStatsComponent.GetCurrentStats().GetStat(static_cast<int32>(inStatType) - 1);
	inStatsComponent.UpdateCurrentStat(inStatType, value + inValue);

	mIsChanage = true;
}

void BuffComponent::ReleaseBuff(StatsComponent& inStatsComponent, const EStatType& inStatType, const float& inValue)
{
	auto findBuff = mBuffs.find(inStatType);
	if (findBuff == mBuffs.end())
	{
		return;
	}

	findBuff->second -= inValue;

	const float value = inStatsComponent.GetCurrentStats().GetStat(static_cast<int32>(inStatType) - 1);
	inStatsComponent.UpdateCurrentStat(inStatType, value - inValue);

	if (findBuff->second == 0.0f)
	{
		mBuffs.erase(inStatType);
	}

	mIsChanage = true;
}
