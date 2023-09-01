#pragma once

using SkillHandlerFunc = std::function<bool(GameRemotePlayerRef&, bool)>;

enum class ESkillID : uint16
{
	Skill_Warrior_Buff			= 1,
	Skill_Warrior_Parrying		= 2,
	Skill_Warrior_ShieldBash	= 3,
	Skill_Warrior_SwordBlow		= 4,

	Skill_Rich_Rise,
	Skill_Rich_Rise_Skeleton,
	Skill_Rich_Rise_DarkKnight,
	Skill_Rich_Blink_Attack,
	Skill_Rich_Blink_Sturn,
	Skill_Rich_Soul_Spear,
	Skill_Rich_Soul_Spark,
	Skill_Rich_Soul_Shackles,
	Skill_Rich_Explosion,
	Skill_Rich_Multi_Castring,
	Skill_Rich_Realm_Of_Death,
	Skill_Rich_Onslaught_Of_Shadows,
	Skill_Rich_Life_Vessel,

	Skill_DarkKnight_Running_Attack,
	Skill_DarkKnight_Charged_Combo_Attack,
	Skill_DarkKnight_Uppercut_Attack,
	Skill_DarkKnight_Swing_Attack,
	Skill_DarkKnight_Swing_And_Slam_Attack,
	Skill_DarkKnight_Hand_And_Sword_Swipe_Attack,
	Skill_DarkKnight_Berserk,
};

bool Skill_INVALID(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed);
bool Skill_Warrior_Buff(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed);
bool Skill_Warrior_Parrying(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed);
bool Skill_Warrior_ShieldBash(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed);
bool Skill_Warrior_SwordBlow(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed);

class SkillHandler
{
public:
	static void Init()
	{
		for (uint16 i = 0; i < UINT16_MAX; ++i)
		{
			mSkillPacketHandlers[i] = Skill_INVALID;
		}

		mSkillPacketHandlers[static_cast<uint16>(ESkillID::Skill_Warrior_Buff)] = [](GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed) { return SkillHandler::HandlePacket(Skill_Warrior_Buff, inGameRemotePlayer, inIsPressed); };
		mSkillPacketHandlers[static_cast<uint16>(ESkillID::Skill_Warrior_Parrying)] = [](GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed) { return SkillHandler::HandlePacket(Skill_Warrior_Parrying, inGameRemotePlayer, inIsPressed); };
		mSkillPacketHandlers[static_cast<uint16>(ESkillID::Skill_Warrior_ShieldBash)] = [](GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed) { return SkillHandler::HandlePacket(Skill_Warrior_ShieldBash, inGameRemotePlayer, inIsPressed); };
		mSkillPacketHandlers[static_cast<uint16>(ESkillID::Skill_Warrior_SwordBlow)] = [](GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed) { return SkillHandler::HandlePacket(Skill_Warrior_SwordBlow, inGameRemotePlayer, inIsPressed); };
	}

	static bool HandlePacket(GameRemotePlayerRef& inGameRemotePlayer, int64 inSkillID, bool inIsPressed)
	{
		return mSkillPacketHandlers[inSkillID](inGameRemotePlayer, inIsPressed);
	}

private:
	template<typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
	{
		return func(inGameRemotePlayer, inIsPressed);
	}

public:
	static SkillHandlerFunc mSkillPacketHandlers[UINT16_MAX];
};

