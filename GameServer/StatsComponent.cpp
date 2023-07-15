#include "pch.h"
#include "StatsComponent.h"

StatsComponent::StatsComponent() : mMaxStats(), mCurrentStats(), mClass(0), mBaseType(EGameDataType::MAX_GAME_DATA), mGrowType(EGameDataType::MAX_GAME_DATA), mStatSyncTime(0), mMaxStatSyncTime(0)
{
}

StatsComponent::~StatsComponent()
{
}

void StatsComponent::SetSyncTime(const int64 inSyncTime)
{
	mMaxStatSyncTime	= inSyncTime;
	mStatSyncTime		= 0;
}

void StatsComponent::InitMaxStats(const Stats& inMaxStats)
{
	mMaxStats = inMaxStats;
	mCurrentStats = inMaxStats;
}

void StatsComponent::InitMaxStats(ActorPtr inActor, const EGameDataType inBaseData, int32 inClass)
{
	WorldPtr world = inActor->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	GameDatasPtr dataManager = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == dataManager)
	{
		return;
	}

	dataManager->GetStats(inBaseData, inClass, mMaxStats);

	mBaseType = inBaseData;
	mClass = inClass;

}

void StatsComponent::InitMaxStats(ActorPtr inActor, const EGameDataType inBaseData, const EGameDataType inGrowData, const int32 inClass, const int32 inLevel)
{
	WorldPtr world = inActor->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	GameDatasPtr dataManager = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == dataManager)
	{
		return;
	}

	if (inBaseData == EGameDataType::MAX_GAME_DATA || inGrowData == EGameDataType::MAX_GAME_DATA)
	{
		return;
	}

	Stats baseStats;
	dataManager->GetStats(inBaseData, inClass, baseStats);

	Stats growStats;
	dataManager->GetStats(inGrowData, inClass, growStats);

	mMaxStats		= baseStats + (growStats * inLevel);
	mCurrentStats	= mMaxStats;
	mBaseType		= inBaseData;
	mGrowType		= inGrowData;
	mClass			= inClass;
}

bool StatsComponent::UpdateStatSync(const int64 inDeltaTime, std::map<EStatType, float>& outUpdateStats)
{
	mStatSyncTime += inDeltaTime;
	if (mStatSyncTime <= mMaxStatSyncTime)
	{
		return false;
	}
	mStatSyncTime = 0;

	std::map<EStatType, float> tempDifferentStats;

	for (int32 index = 0; index < MAX_STATS_NUM; ++index)
	{
		float maxStat = mMaxStats.GetStat(index);
		float curStat = mCurrentStats.GetStat(index);
		if (maxStat != curStat)
		{
			std::pair<EStatType, float> differentStat = std::make_pair(static_cast<EStatType>(index + 1), curStat);
			tempDifferentStats.insert(differentStat);
		}
	}

	outUpdateStats.swap(tempDifferentStats);
	return outUpdateStats.size();
}

void StatsComponent::UpdateMaxStats(ActorPtr inActor, const int32 inLevel)
{
	WorldPtr world = inActor->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	GameDatasPtr dataManager = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == dataManager)
	{
		return;
	}

	if (mBaseType == EGameDataType::MAX_GAME_DATA || mGrowType == EGameDataType::MAX_GAME_DATA)
	{
		return;
	}

	Stats baseStats;
	dataManager->GetStats(mBaseType, mClass, baseStats);

	Stats growStats;
	dataManager->GetStats(mGrowType, mClass, growStats);

	mMaxStats = baseStats + (growStats * inLevel);

}

void StatsComponent::UpdateCurrentStat(const EStatType inStatType, const float inValue)
{
	switch (inStatType)
	{
	case Stat_Unspecified:
		break;
	case Stat_ArmorPenetration:
		mCurrentStats.SetArmorPenetration(inValue);
		break;
	case Stat_AttackDamage:
		mCurrentStats.SetAttackSpeed(inValue);
		break;
	case Stat_AttackSpeed:
		mCurrentStats.SetAttackSpeed(inValue);
		break;
	case Stat_CriticalStrikeChance:
		mCurrentStats.SetCriticalStrikeChance(inValue);
		break;
	case Stat_CirticalStrikeDamage:
		mCurrentStats.SetCirticalStrikeDamage(inValue);
		break;
	case Stat_LifeSteal:
		mCurrentStats.SetLifeSteal(inValue);
		break;
	case Stat_AbilityPower:
		mCurrentStats.SetAbilityPower(inValue);
		break;
	case Stat_MagePenetration:
		mCurrentStats.SetMagePenetration(inValue);
		break;
	case Stat_Omnivamp:
		mCurrentStats.SetOmnivamp(inValue);
		break;
	case Stat_PhysicalVamp:
		mCurrentStats.SetPhysicalVamp(inValue);
		break;
	case Stat_Armor:
		mCurrentStats.SetArmor(inValue);
		break;
	case Stat_HealAndShieldPower:
		mCurrentStats.SetHealAndShieldPower(inValue);
		break;
	case Stat_Health:
		mCurrentStats.SetHealth(inValue);
		break;
	case Stat_HealthRegeneration:
		mCurrentStats.SetHealthRegeneration(inValue);
		break;
	case Stat_MagicResistance:
		mCurrentStats.SetMagicResistance(inValue);
		break;
	case Stat_Tenacity:
		mCurrentStats.SetTenacity(inValue);
		break;
	case Stat_SlowResist:
		mCurrentStats.SetSlowResist(inValue);
		break;
	case Stat_AbilityHaste:
		mCurrentStats.SetAbilityHaste(inValue);
		break;
	case Stat_Mana:
		mCurrentStats.SetMana(inValue);
		break;
	case Stat_ManaRegeneration:
		mCurrentStats.SetManaRegeneration(inValue);
		break;
	case Stat_MovementSpeed:
		mCurrentStats.SetMovementSpeed(inValue);
		break;
	case Stat_Range:
		mCurrentStats.SetRange(inValue);
		break;
	default:
		break;
	}

	auto findStat = mUpdateStats.find(inStatType);
	if (findStat == mUpdateStats.end())
	{
		std::pair<EStatType, float> updateStat = std::make_pair(inStatType, inValue);
		mUpdateStats.insert(updateStat);
	}
	else
	{
		findStat->second = inValue;
	}

}

const Stats& StatsComponent::GetMaxStats() const
{
    return mMaxStats;
}

const Stats& StatsComponent::GetCurrentStats() const
{
    return mCurrentStats;
}

bool StatsComponent::GetUpdateStats(std::map<EStatType, float>& outUpdateStats)
{
	if (mUpdateStats.size() == 0)
	{
		return false;
	}

	outUpdateStats.swap(mUpdateStats);

	if (mUpdateStats.size() != 0)
	{
		mUpdateStats.clear();
	}

	return outUpdateStats.size();
}