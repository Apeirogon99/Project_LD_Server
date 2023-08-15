#include "pch.h"
#include "SkillHandler.h"

SkillHandlerFunc SkillHandler::mSkillPacketHandlers[UINT16_MAX];

bool Skill_INVALID(GameRemotePlayerRef& inGameRemotePlayer)
{
    return false;
}

bool Skill_Warrior_Buff(GameRemotePlayerRef& inGameRemotePlayer)
{
    printf("Use Skill_Warrior_Buff\n");

    GameRemotePlayerPtr remotePlayer = inGameRemotePlayer.lock();

    GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
    if (nullptr == world)
    {
        return false;
    }

    PlayerCharacterPtr character = remotePlayer->GetCharacter();
    if (nullptr == character)
    {
        return false;
    }
    SkillComponent& skillComponent = character->GetSkillComponent();

    if (false == skillComponent.CanUseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff)))
    {
        return false;
    }
    skillComponent.UseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff), 5000);

    
    ActorPtr newActor = world->SpawnActor<WarriorBuff>(remotePlayer->GetGameObjectRef(), character->GetLocation(), FRotator(), Scale(1.0f, 1.0f, 1.0f));
    if (nullptr == newActor)
    {
        return false;
    }

    std::shared_ptr<WarriorBuff> warriorBuff = std::static_pointer_cast<WarriorBuff>(newActor);
    if (nullptr == warriorBuff)
    {
        return false;
    }
    warriorBuff->SetDuration(3000);

    return true;
}

bool Skill_Warrior_Parrying(GameRemotePlayerRef& inGameRemotePlayer)
{
    printf("Use Skill_Warrior_Parrying\n");
    return true;
}

bool Skill_Warrior_ShieldBash(GameRemotePlayerRef& inGameRemotePlayer)
{
    printf("Use Skill_Warrior_ShieldBash\n");
    return true;
}

bool Skill_Warrior_SwordBlow(GameRemotePlayerRef& inGameRemotePlayer)
{
    printf("Use Skill_Warrior_SwordBlow\n");
    return true;
}
