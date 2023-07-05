#include "pch.h"
#include "StatsComponent.h"

StatsComponent::StatsComponent() : mMaxStats(), mCurrentStats(), mClass(0), mBaseType(EGameDataType::MAX_GAME_DATA), mGrowType(EGameDataType::MAX_GAME_DATA)
{
}

StatsComponent::~StatsComponent()
{
}

void StatsComponent::InitMaxStats(const Stats& inMaxStats)
{
	mMaxStats = inMaxStats;
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

	Stats baseStats;
	dataManager->GetStats(mBaseType, mClass, baseStats);

	Stats growStats;
	dataManager->GetStats(mGrowType, mClass, growStats);

	mMaxStats = baseStats + (growStats * inLevel);
	mBaseType = inBaseData;
	mGrowType = inGrowData;
	mClass = inClass;
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

void StatsComponent::UpdateCurrentStat(const EStat inStatType, const float inValue)
{
	switch (inStatType)
	{
	case armor_penetration:
		mCurrentStats.SetArmorPenetration(inValue);
		break;
	case attack_damage:
		mCurrentStats.SetAttackSpeed(inValue);
		break;
	case attack_speed:
		mCurrentStats.SetAttackSpeed(inValue);
		break;
	case critical_strike_chance:
		mCurrentStats.SetCriticalStrikeChance(inValue);
		break;
	case cirtical_strike_damage:
		mCurrentStats.SetCirticalStrikeDamage(inValue);
		break;
	case life_steal:
		mCurrentStats.SetLifeSteal(inValue);
		break;
	case ability_power:
		mCurrentStats.SetAbilityPower(inValue);
		break;
	case mage_penetration:
		mCurrentStats.SetMagePenetration(inValue);
		break;
	case omnivamp:
		mCurrentStats.SetOmnivamp(inValue);
		break;
	case physical_vamp:
		mCurrentStats.SetPhysicalVamp(inValue);
		break;
	case armor:
		mCurrentStats.SetArmor(inValue);
		break;
	case heal_and_shield_power:
		mCurrentStats.SetHealAndShieldPower(inValue);
		break;
	case health:
		mCurrentStats.SetHealth(inValue);
		break;
	case health_regeneration:
		mCurrentStats.SetHealthRegeneration(inValue);
		break;
	case magic_resistance:
		mCurrentStats.SetMagicResistance(inValue);
		break;
	case tenacity:
		mCurrentStats.SetTenacity(inValue);
		break;
	case slow_resist:
		mCurrentStats.SetSlowResist(inValue);
		break;
	case ability_haste:
		mCurrentStats.SetAbilityHaste(inValue);
		break;
	case mana:
		mCurrentStats.SetMana(inValue);
		break;
	case mana_regeneration:
		mCurrentStats.SetManaRegeneration(inValue);
		break;
	case movement_speed:
		mCurrentStats.SetMovementSpeed(inValue);
		break;
	case range:
		mCurrentStats.SetRange(inValue);
		break;
	default:
		break;
	}
}

const Stats& StatsComponent::GetMaxStats()
{
    return mMaxStats;
}

const Stats& StatsComponent::GetCurrentStats()
{
    return mCurrentStats;
}
