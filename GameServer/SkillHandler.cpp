#include "pch.h"
#include "SkillHandler.h"

SkillHandlerFunc SkillHandler::mSkillPacketHandlers[UINT16_MAX];

bool Skill_INVALID(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    return false;
}

bool Skill_Warrior_Buff(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    printf("Use Skill_Warrior_Buff\n");
    if (false == inIsPressed)
    {
        return true;
    }

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
    FRotator rotation = character->GetRotation();

    if (false == skillComponent.CanUseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff)))
    {
        return true;
    }
    
    ActorPtr newActor = world->SpawnActor<WarriorBuff>(remotePlayer->GetGameObjectRef(), location, rotation, Scale(1.0f, 1.0f, 1.0f));
    if (nullptr == newActor)
    {
        return false;
    }

    std::shared_ptr<WarriorBuff> warriorBuff = std::static_pointer_cast<WarriorBuff>(newActor);
    if (nullptr == warriorBuff)
    {
        return false;
    }
    skillComponent.UseSkill(warriorBuff, static_cast<int32>(ESkillID::Skill_Warrior_Buff), 60000);
    skillComponent.SetActiveSkill(warriorBuff);

    warriorBuff->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff), world->GetWorldTime());
    warriorBuff->Active();
    warriorBuff->DeActive(40000);

    return true;
}

bool Skill_Warrior_Parrying(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    printf("Use Skill_Warrior_Parrying\n");
    if (false == inIsPressed)
    {
        return true;
    }

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
    FRotator rotation = character->GetRotation();

    if (false == skillComponent.CanUseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Parrying)))
    {
        return true;
    }

    ActorPtr newActor = world->SpawnActor<WarriorParrying>(remotePlayer->GetGameObjectRef(), location, rotation, Scale(1.0f, 1.0f, 1.0f));
    if (nullptr == newActor)
    {
        return false;
    }

    std::shared_ptr<WarriorParrying> warriorParrying = std::static_pointer_cast<WarriorParrying>(newActor);
    if (nullptr == warriorParrying)
    {
        return false;
    }
    skillComponent.UseSkill(warriorParrying, static_cast<int32>(ESkillID::Skill_Warrior_Parrying), 2000);
    skillComponent.SetActiveSkill(warriorParrying);

    warriorParrying->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_Parrying), world->GetWorldTime());
    warriorParrying->Active();

    return true;
}

bool Skill_Warrior_ShieldBash(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    printf("Use Skill_Warrior_ShieldBash\n");
    if (false == inIsPressed)
    {
        return true;
    }

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
    FRotator rotation = character->GetRotation();

    if (false == skillComponent.CanUseSkill(static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash)))
    {
        return true;
    }

    ActorPtr newActor = world->SpawnActor<WarriorShieldBash>(remotePlayer->GetGameObjectRef(), location, rotation, Scale(1.0f, 1.0f, 1.0f));
    if (nullptr == newActor)
    {
        return false;
    }

    std::shared_ptr<WarriorShieldBash> warriorShieldBash = std::static_pointer_cast<WarriorShieldBash>(newActor);
    if (nullptr == warriorShieldBash)
    {
        return false;
    }
    skillComponent.UseSkill(warriorShieldBash, static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash), 10000);
    skillComponent.SetActiveSkill(warriorShieldBash);

    warriorShieldBash->SetWarriorShieldBash(10.0f, -50.0f, 2000, 3000, 2000, 2100);

    warriorShieldBash->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash), world->GetWorldTime());
    warriorShieldBash->Active();
    warriorShieldBash->DeActive(2690);

    return true;
}

bool Skill_Warrior_SwordBlow(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
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
    FRotator rotation = character->GetRotation();

    if (true == inIsPressed)
    {
        if (false == skillComponent.CanUseSkill(static_cast<int32>(ESkillID::Skill_Warrior_SwordBlow)))
        {
            return true;
        }

        ActorPtr newActor = world->SpawnActor<WarriorSwordBlow>(remotePlayer->GetGameObjectRef(), location, rotation, Scale(1.0f, 1.0f, 1.0f));
        if (nullptr == newActor)
        {
            return false;
        }

        std::shared_ptr<WarriorSwordBlow> warriorSwordBlow = std::static_pointer_cast<WarriorSwordBlow>(newActor);
        if (nullptr == warriorSwordBlow)
        {
            return false;
        }
        skillComponent.SetActiveSkill(warriorSwordBlow);

        warriorSwordBlow->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_SwordBlow), world->GetWorldTime());
        warriorSwordBlow->Active();
    }
    else
    {

        ActiveSkillPtr activeSkill = skillComponent.GetActiveSkill().lock();
        if (nullptr == activeSkill)
        {
            return true;
        }

        if (activeSkill->GetSkillID() != static_cast<int32>(ESkillID::Skill_Warrior_SwordBlow))
        {
            return true;
        }

        std::shared_ptr<WarriorSwordBlow> warriorSwordBlow = std::static_pointer_cast<WarriorSwordBlow>(activeSkill);
        if (false == warriorSwordBlow->IsCharge())
        {
            return true;
        }
        skillComponent.UseSkill(warriorSwordBlow, static_cast<int32>(ESkillID::Skill_Warrior_SwordBlow), 1000);

        warriorSwordBlow->Active();
        warriorSwordBlow->DeActive(2000);
    }

    return true;
}
