#include "pch.h"
#include "SkillHandler.h"

SkillHandlerFunc SkillHandler::mSkillPacketHandlers[UINT16_MAX];

bool Skill_INVALID(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    return false;
}

bool Skill_Warrior_Buff(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
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
   
    int32 result = character->UseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff), 250.0f);
    if (static_cast<int32>(EDCommonErrorType::SUCCESS) != result)
    {
        return false;
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
    skillComponent.UseSkill(warriorBuff, static_cast<int32>(ESkillID::Skill_Warrior_Buff), 10000);
    skillComponent.SetActiveSkill(warriorBuff);

    warriorBuff->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_Buff), world->GetWorldTime());
    warriorBuff->Active();
    warriorBuff->DeActive(40000);

    return true;
}

bool Skill_Warrior_Parrying(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
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

    int32 result = character->UseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Parrying), 75.0f);
    if (static_cast<int32>(EDCommonErrorType::SUCCESS) != result)
    {
        return false;
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
    skillComponent.UseSkill(warriorParrying, static_cast<int32>(ESkillID::Skill_Warrior_Parrying), 7000);
    skillComponent.SetActiveSkill(warriorParrying);

    warriorParrying->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_Parrying), world->GetWorldTime());
    warriorParrying->PushTask(world->GetNextWorldTime(), &WarriorParrying::Active);
    warriorParrying->DeActive(1200);

    return true;
}

bool Skill_Warrior_ShieldBash(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
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

    int32 result = character->UseSkill(static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash), 175.0f);
    if (static_cast<int32>(EDCommonErrorType::SUCCESS) != result)
    {
        return false;
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
    skillComponent.UseSkill(warriorShieldBash, static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash), 15000);
    skillComponent.SetActiveSkill(warriorShieldBash);

    warriorShieldBash->SetWarriorShieldBash(200.0f, -50.0f);

    warriorShieldBash->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash), world->GetWorldTime());
    warriorShieldBash->Active();
    warriorShieldBash->DeActive(7300);

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
        int32 result = character->UseSkill(static_cast<int32>(ESkillID::Skill_Warrior_SwordBlow), 200.0f);
        if (static_cast<int32>(EDCommonErrorType::SUCCESS) != result)
        {
            return false;
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

        if (false == skillComponent.CanUseSkill(static_cast<int32>(ESkillID::Skill_Warrior_SwordBlow)))
        {
            printf("Can't Use release Skill_Warrior_SwordBlow\n");
            return false;
        }
        printf("Use release Skill_Warrior_SwordBlow\n");

        std::shared_ptr<WarriorSwordBlow> warriorSwordBlow = std::static_pointer_cast<WarriorSwordBlow>(activeSkill);
        if (false == warriorSwordBlow->IsCharge())
        {
            return true;
        }
        skillComponent.UseSkill(warriorSwordBlow, static_cast<int32>(ESkillID::Skill_Warrior_SwordBlow), 20000);

        warriorSwordBlow->PushTask(world->GetNextWorldTime(), &WarriorSwordBlow::Active);
    }

    return true;
}

bool Skill_Warrior_Dash(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{

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
    const int64 nextWorldTime = world->GetNextWorldTime();

    PlayerCharacterPtr character = remotePlayer->GetCharacter();
    if (nullptr == character)
    {
        return false;
    }
    SkillComponent& skillComponent = character->GetSkillComponent();
    FVector	location = character->GetMovementComponent().GetCurrentLocation(character->GetActorPtr());
    FRotator rotation = character->GetRotation();

    int32 result = character->UseSkill(static_cast<int32>(ESkillID::Skill_Warrior_Dash), 75.0f);
    if (static_cast<int32>(EDCommonErrorType::SUCCESS) != result)
    {
        return false;
    }

    ActorPtr newActor = world->SpawnActor<WarriorDash>(remotePlayer->GetGameObjectRef(), location, rotation, Scale(1.0f, 1.0f, 1.0f));
    if (nullptr == newActor)
    {
        return false;
    }

    std::shared_ptr<WarriorDash> warriorDash = std::static_pointer_cast<WarriorDash>(newActor);
    if (nullptr == warriorDash)
    {
        return false;
    }
    skillComponent.UseSkill(warriorDash, static_cast<int32>(ESkillID::Skill_Warrior_Dash), 5000);
    skillComponent.SetActiveSkill(warriorDash);

    warriorDash->SetActiveSkill(static_cast<int32>(ESkillID::Skill_Warrior_Dash), world->GetWorldTime());
    warriorDash->SetWarriorDash(400.0f, 4000.0f);
    warriorDash->PushTask(nextWorldTime, &WarriorDash::Active);
    return true;
}
