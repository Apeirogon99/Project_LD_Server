#include "pch.h"
#include "StatsComponent.h"

StatsComponent::StatsComponent() : mIsChanageStats(false), mMaxStats(), mCurrentStats(), mClass(0), mBaseType(EGameDataType::MAX_GAME_DATA), mGrowType(EGameDataType::MAX_GAME_DATA), mStatSyncTime(0), mMaxStatSyncTime(0), mUpdateStatTime(0)
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

Stats StatsComponent::LoadMaxStats(ActorPtr inActor) const
{
	WorldPtr world = inActor->GetWorld().lock();
	if (nullptr == world)
	{
		return Stats();
	}

	PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(inActor);
	if (nullptr == character)
	{
		return Stats();
	}

	LevelComponent& levelComponent = character->GetLevelComponent();
	const int32& level = levelComponent.GetLevel();

	GameDatasPtr dataManager = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == dataManager)
	{
		return Stats();
	}

	Stats baseStats;
	dataManager->GetStats(mBaseType, mClass, baseStats);

	Stats growStats;
	dataManager->GetStats(mGrowType, mClass, growStats);

	return baseStats + (growStats * level);;
}

bool StatsComponent::ExtractDifferentMaxStats(std::map<EStatType, float>& outDifferentStats)
{

	for (int32 index = 0; index < MAX_STATS_NUM; ++index)
	{
		float maxStat = mMaxStats.GetStat(index);
		float curStat = mCurrentStats.GetStat(index);
		if (maxStat != curStat)
		{
			std::pair<EStatType, float> differentStat = std::make_pair(static_cast<EStatType>(index + 1), curStat);
			outDifferentStats.insert(differentStat);
		}
	}

	return outDifferentStats.size();
}

bool StatsComponent::ExtractChanageMaxStats(std::map<EStatType, float>& outUpdateStats)
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

	mIsChanageStats = false;
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
	case EStatType::Stat_Unspecified:
		break;
	case EStatType::Stat_ArmorPenetration:
		mCurrentStats.SetArmorPenetration(inValue);
		break;
	case EStatType::Stat_AttackDamage:
		mCurrentStats.SetAttackDamage(inValue);
		break;
	case EStatType::Stat_AttackSpeed:
		mCurrentStats.SetAttackSpeed(inValue);
		break;
	case EStatType::Stat_CriticalStrikeChance:
		mCurrentStats.SetCriticalStrikeChance(inValue);
		break;
	case EStatType::Stat_CirticalStrikeDamage:
		mCurrentStats.SetCirticalStrikeDamage(inValue);
		break;
	case EStatType::Stat_LifeSteal:
		mCurrentStats.SetLifeSteal(inValue);
		break;
	case EStatType::Stat_AbilityPower:
		mCurrentStats.SetAbilityPower(inValue);
		break;
	case EStatType::Stat_MagePenetration:
		mCurrentStats.SetMagePenetration(inValue);
		break;
	case EStatType::Stat_Omnivamp:
		mCurrentStats.SetOmnivamp(inValue);
		break;
	case EStatType::Stat_PhysicalVamp:
		mCurrentStats.SetPhysicalVamp(inValue);
		break;
	case EStatType::Stat_Armor:
		mCurrentStats.SetArmor(inValue);
		break;
	case EStatType::Stat_HealAndShieldPower:
		mCurrentStats.SetHealAndShieldPower(inValue);
		break;
	case EStatType::Stat_Health:
		mCurrentStats.SetHealth(inValue);
		break;
	case EStatType::Stat_HealthRegeneration:
		mCurrentStats.SetHealthRegeneration(inValue);
		break;
	case EStatType::Stat_MagicResistance:
		mCurrentStats.SetMagicResistance(inValue);
		break;
	case EStatType::Stat_Tenacity:
		mCurrentStats.SetTenacity(inValue);
		break;
	case EStatType::Stat_SlowResist:
		mCurrentStats.SetSlowResist(inValue);
		break;
	case EStatType::Stat_AbilityHaste:
		mCurrentStats.SetAbilityHaste(inValue);
		break;
	case EStatType::Stat_Mana:
		mCurrentStats.SetMana(inValue);
		break;
	case EStatType::Stat_ManaRegeneration:
		mCurrentStats.SetManaRegeneration(inValue);
		break;
	case EStatType::Stat_MovementSpeed:
		mCurrentStats.SetMovementSpeed(inValue);
		break;
	case EStatType::Stat_Range:
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

	mIsChanageStats = true;
}

void StatsComponent::UpdateCurrentStat(const Stats& inStats)
{

	for (int32 index = 0; index < MAX_STATS_NUM; ++index)
	{
		const float&	 value	= inStats.GetStats()[index];
		const EStatType& type	= static_cast<EStatType>(index + 1);

		if (value == 0.0f)
		{
			continue;
		}

		auto findStat = mUpdateStats.find(type);
		if (findStat == mUpdateStats.end())
		{
			std::pair<EStatType, float> updateStat = std::make_pair(type, value);
			mUpdateStats.insert(updateStat);
		}
		else
		{
			findStat->second = value;
		}
	}

	mIsChanageStats = true;
}

void StatsComponent::LevelUp(GameWorldPtr inGameWorld, const int32 inLevel)
{
	if (nullptr == inGameWorld)
	{
		return;
	}

	GameDatasPtr dataManager = std::static_pointer_cast<GameDatas>(inGameWorld->GetDatas());
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
	mCurrentStats = mMaxStats;
}

const Stats& StatsComponent::GetMaxStats() const
{
    return mMaxStats;
}

const Stats& StatsComponent::GetCurrentStats() const
{
    return mCurrentStats;
}

void StatsComponent::UpdateStats(const int64 inDeletaTime)
{
	mUpdateStatTime += inDeletaTime;
	if (mUpdateStatTime < GAME_TICK)
	{
		return;
	}
	mUpdateStatTime = 0;

	float curHealth = mCurrentStats.GetHealth();
	float curMana	= mCurrentStats.GetMana();

	float healthRegenration = mCurrentStats.GetHealthRegeneration();
	float manaRegenration	= mCurrentStats.GetManaRegeneration();

	UpdateCurrentStat(EStatType::Stat_Health, std::clamp(curHealth + healthRegenration, 0.0f, mMaxStats.GetHealth()));
	UpdateCurrentStat(EStatType::Stat_Mana, std::clamp(curMana + manaRegenration, 0.0f, mMaxStats.GetMana()));
}

bool StatsComponent::IsChanageStats(const int64 inDeletaTime)
{
	mStatSyncTime += inDeletaTime;
	if (mStatSyncTime < mMaxStatSyncTime)
	{
		return false;
	}
	mStatSyncTime = 0;

	if (false == mIsChanageStats)
	{
		return false;
	}

	return true;
}
