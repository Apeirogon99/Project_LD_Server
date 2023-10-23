#include "pch.h"
#include "BuffComponent.h"

BuffComponent::BuffComponent() : mIsChanage(false)
{
}

BuffComponent::~BuffComponent()
{
}

void BuffComponent::Init(GameRemotePlayerRef inOwner)
{
	mOwner = inOwner;
}

void BuffComponent::ReapplyBuff(StatsComponent& inStatsComponent)
{
	for (auto buff : mBuffs)
	{
		const float value = inStatsComponent.GetCurrentStats().GetStat(static_cast<int32>(buff.first) - 1);
		inStatsComponent.UpdateCurrentStat(buff.first, value + buff.second);
	}
}

void BuffComponent::PushBuff(StatsComponent& inStatsComponent, const EBuffType& inBuffID, const EStatType& inStatType, const float& inValue)
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

	PlayerCharacterPtr& character = owner->GetCharacter();
	if (nullptr == character)
	{
		return;
	}

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

	ApplyBuff(character, inStatType, value + inValue);

	{
		Protocol::S2C_PushBuff pushBuffPacket;
		pushBuffPacket.set_remote_id(owner->GetGameObjectID());
		pushBuffPacket.set_buff_id(static_cast<int32>(inBuffID));

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, pushBuffPacket);
		owner->BrodcastPlayerViewers(sendBuffer);
		//playerState->BroadcastPlayerMonitors(sendBuffer);
	}
}

void BuffComponent::ReleaseBuff(StatsComponent& inStatsComponent, const EBuffType& inBuffID, const EStatType& inStatType, const float& inValue)
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

	PlayerCharacterPtr& character = owner->GetCharacter();
	if (nullptr == character)
	{
		return;
	}

	auto findBuff = mBuffs.find(inStatType);
	if (findBuff == mBuffs.end())
	{
		return;
	}

	findBuff->second -= inValue;

	const float value = inStatsComponent.GetCurrentStats().GetStat(static_cast<int32>(inStatType) - 1);
	inStatsComponent.UpdateCurrentStat(inStatType, value - inValue);

	if (findBuff->second <= 0.0f)
	{
		mBuffs.erase(inStatType);
	}

	mIsChanage = true;

	ApplyBuff(character, inStatType, value - inValue);

	{
		Protocol::S2C_ReleaseBuff releaseBuffPacket;
		releaseBuffPacket.set_remote_id(owner->GetGameObjectID());
		releaseBuffPacket.set_buff_id(static_cast<int32>(inBuffID));

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, releaseBuffPacket);
		owner->BrodcastPlayerViewers(sendBuffer);
		//playerState->BroadcastPlayerMonitors(sendBuffer);
	}
}

void BuffComponent::ApplyBuff(PlayerCharacterPtr inPlayerCharacter, const EStatType& inStatType, const float& inValue)
{

	switch (inStatType)
	{
	case EStatType::Stat_Unspecified:
		break;
	case EStatType::Stat_ArmorPenetration:
		break;
	case EStatType::Stat_AttackDamage:
		break;
	case EStatType::Stat_AttackSpeed:
		break;
	case EStatType::Stat_CriticalStrikeChance:
		break;
	case EStatType::Stat_CirticalStrikeDamage:
		break;
	case EStatType::Stat_LifeSteal:
		break;
	case EStatType::Stat_AbilityPower:
		break;
	case EStatType::Stat_MagePenetration:
		break;
	case EStatType::Stat_Omnivamp:
		break;
	case EStatType::Stat_PhysicalVamp:
		break;
	case EStatType::Stat_Armor:
		break;
	case EStatType::Stat_HealAndShieldPower:
		break;
	case EStatType::Stat_Health:
		break;
	case EStatType::Stat_HealthRegeneration:
		break;
	case EStatType::Stat_MagicResistance:
		break;
	case EStatType::Stat_Tenacity:
		break;
	case EStatType::Stat_SlowResist:
		break;
	case EStatType::Stat_AbilityHaste:
		break;
	case EStatType::Stat_Mana:
		break;
	case EStatType::Stat_ManaRegeneration:
		break;
	case EStatType::Stat_MovementSpeed:
		inPlayerCharacter->SetVelocity(FVector(inValue, inValue, inValue));
		break;
	case EStatType::Stat_Range:
		break;
	default:
		break;
	}
}
