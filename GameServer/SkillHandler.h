#pragma once

using SkillHandlerFunc = std::function<bool(GameRemotePlayerRef&)>;

enum class ESkillID : uint16
{
	Skill_Warrior_Buff			= 1,
	Skill_Warrior_Parrying		= 2,
	Skill_Warrior_ShieldBash	= 3,
	Skill_Warrior_SwordBlow		= 4,
};

bool Skill_INVALID(GameRemotePlayerRef& inGameRemotePlayer);
bool Skill_Warrior_Buff(GameRemotePlayerRef& inGameRemotePlayer);
bool Skill_Warrior_Parrying(GameRemotePlayerRef& inGameRemotePlayer);
bool Skill_Warrior_ShieldBash(GameRemotePlayerRef& inGameRemotePlayer);
bool Skill_Warrior_SwordBlow(GameRemotePlayerRef& inGameRemotePlayer);

class SkillHandler
{
public:
	static void Init()
	{
		for (uint16 i = 0; i < UINT16_MAX; ++i)
		{
			mSkillPacketHandlers[i] = Skill_INVALID;
		}

		mSkillPacketHandlers[static_cast<uint16>(ESkillID::Skill_Warrior_Buff)] = [](GameRemotePlayerRef& inGameRemotePlayer) { return SkillHandler::HandlePacket(Skill_Warrior_Buff, inGameRemotePlayer); };
		mSkillPacketHandlers[static_cast<uint16>(ESkillID::Skill_Warrior_Parrying)] = [](GameRemotePlayerRef& inGameRemotePlayer) { return SkillHandler::HandlePacket(Skill_Warrior_Parrying, inGameRemotePlayer); };
		mSkillPacketHandlers[static_cast<uint16>(ESkillID::Skill_Warrior_ShieldBash)] = [](GameRemotePlayerRef& inGameRemotePlayer) { return SkillHandler::HandlePacket(Skill_Warrior_ShieldBash, inGameRemotePlayer); };
		mSkillPacketHandlers[static_cast<uint16>(ESkillID::Skill_Warrior_SwordBlow)] = [](GameRemotePlayerRef& inGameRemotePlayer) { return SkillHandler::HandlePacket(Skill_Warrior_SwordBlow, inGameRemotePlayer); };
	}

	static bool HandlePacket(GameRemotePlayerRef& inGameRemotePlayer, int64 inSkillID)
	{
		return mSkillPacketHandlers[inSkillID](inGameRemotePlayer);
	}

private:
	template<typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, GameRemotePlayerRef& inGameRemotePlayer)
	{
		return func(inGameRemotePlayer);
	}

public:
	static SkillHandlerFunc mSkillPacketHandlers[UINT16_MAX];
};

