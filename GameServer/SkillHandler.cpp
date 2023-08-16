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
    FVector	location = character->GetMovementComponent().GetCurrentLocation(character->GetActorPtr());

    if (false == skillComponent.CanUseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff)))
    {
        return false;
    }
    skillComponent.UseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff), 60000);

    
    ActorPtr newActor = world->SpawnActor<WarriorBuff>(remotePlayer->GetGameObjectRef(), location, FRotator(), Scale(1.0f, 1.0f, 1.0f));
    if (nullptr == newActor)
    {
        return false;
    }

    std::shared_ptr<WarriorBuff> warriorBuff = std::static_pointer_cast<WarriorBuff>(newActor);
    if (nullptr == warriorBuff)
    {
        return false;
    }
    warriorBuff->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff), world->GetWorldTime());
    warriorBuff->Active();
    warriorBuff->DeActive(40000);

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
    GameRemotePlayerPtr remotePlayer = inGameRemotePlayer.lock();

    GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
    if (nullptr == world)
    {
        return false;
    }
    const int64 worldTime = world->GetWorldTime();

    PlayerCharacterPtr character = remotePlayer->GetCharacter();
    if (nullptr == character)
    {
        return false;
    }
    SkillComponent& skillComponent = character->GetSkillComponent();
    FVector	location = character->GetMovementComponent().GetCurrentLocation(character->GetActorPtr());

    if (false == skillComponent.CanUseSkill(static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash)))
    {
        return false;
    }
    skillComponent.UseSkill(static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash), 10000);

    ActorPtr newActor = world->SpawnActor<WarriorShieldBash>(remotePlayer->GetGameObjectRef(), location, FRotator(), Scale(1.0f, 1.0f, 1.0f));
    if (nullptr == newActor)
    {
        return false;
    }

    std::shared_ptr<WarriorShieldBash> warriorShieldBash = std::static_pointer_cast<WarriorShieldBash>(newActor);
    if (nullptr == warriorShieldBash)
    {
        return false;
    }
    warriorShieldBash->SetWarriorShieldBash(200.0f, 500.0f, 10.0f, -50.0f, 1500, 2000, 1000);

    warriorShieldBash->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash), world->GetWorldTime());
    warriorShieldBash->Active();
    warriorShieldBash->DeActive(1500);

    return true;
}

bool Skill_Warrior_SwordBlow(GameRemotePlayerRef& inGameRemotePlayer)
{
    printf("Use Skill_Warrior_SwordBlow\n");
    return true;
}
