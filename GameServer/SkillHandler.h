#pragma once

using SkillHandlerFunc = std::function<bool(GameRemotePlayerRef&, bool)>;

enum class ESkillID : uint16
{
	Skill_Warrior_Buff			= 1,
	Skill_Warrior_Parrying		= 2,
	Skill_Warrior_ShieldBash	= 3,
	Skill_Warrior_SwordBlow		= 4,
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

