#include "pch.h"
#include "WarriorDash.h"

WarriorDash::WarriorDash() : ActiveSkill(L"WarriorDash"), mDashDistance(300.0f), mInitDashSpeed(1000.0f), mDashSpeed(1000.0f)
{
}

WarriorDash::~WarriorDash()
{
}

void WarriorDash::OnInitialization()
{
    //this->BeginCastingSkill();
}

void WarriorDash::OnDestroy()
{
    GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }
    const int64& worldTime = world->GetWorldTime();

    GameRemotePlayerPtr owner = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
    if (nullptr == owner)
    {
        return;
    }

    PlayerCharacterPtr instigated = std::static_pointer_cast<PlayerCharacter>(owner->GetCharacter());
    if (nullptr == instigated)
    {
        return;
    }

    StatsComponent& playerStats = instigated->GetStatComponent();
    BuffComponent& playerbuff = instigated->GetBuffComponent();
    playerbuff.ReleaseBuff(playerStats, EStatType::Stat_MovementSpeed, mDashSpeed);

    //this->EndCastingSkill();

	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);

}

void WarriorDash::OnTick(const int64 inDeltaTime)
{
}

bool WarriorDash::IsValid()
{
    GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
    if (nullptr == world)
    {
        return false;
    }

    GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
    if (nullptr == remotePlayer)
    {
        if (false == world->IsValidActor(this->GetGameObjectID()))
        {
            return false;
        }

        bool ret = world->DestroyActor(this->GetGameObjectID());
        if (false == ret)
        {
            this->GameObjectLog(L"Can't destroy skill\n");
        }
        return ret;
    }

    return true;
}

void WarriorDash::Active()
{
    GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }
    const int64& worldTime = world->GetWorldTime();

    GameRemotePlayerPtr owner = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
    if (nullptr == owner)
    {
        return;
    }

    PlayerCharacterPtr instigated = std::static_pointer_cast<PlayerCharacter>(owner->GetCharacter());
    if (nullptr == instigated)
    {
        return;
    }

    const FVector foward = instigated->GetRotation().GetForwardVector();
    const float radius = instigated->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();

    const Location characterFoward = instigated->GetLocation() + (foward * radius);
    Location destinationLocation = characterFoward + (foward * mDashDistance);

    LineTrace lineTrace(instigated->GetActorPtr(), characterFoward, destinationLocation, true);

    std::vector<FVector> impactLocation;
    bool result = world->FindObstructionIntersection(lineTrace, static_cast<uint8>(EActorType::Obstruction), impactLocation, 1);
    if (true == result)
    {
        printf("ImpatLocation : "); impactLocation.at(0).ToString();
        destinationLocation = impactLocation.at(0);
    }
    destinationLocation = destinationLocation - (foward * radius);

    printf("DESTINATION : "); destinationLocation.ToString();

    const float debugDuration = 1.0f;
    float distance = FVector::Distance2D(instigated->GetLocation(), destinationLocation) / 2.0f;
    PacketUtils::DebugDrawBox(this->GetPlayerViewers(), instigated->GetLocation(), destinationLocation, FVector(distance, 1.0f, 1.0f), debugDuration);
    PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), (instigated->GetLocation() + destinationLocation) / 2.0f, distance, debugDuration);


    instigated->GetMovementComponent().SetNewDestination(instigated->GetActorPtr(), instigated->GetLocation(), destinationLocation, worldTime, 0.0f);

    float time = (mDashDistance / mInitDashSpeed) * 1000.0f;
    this->DeActive(static_cast<int64>(time));

    Protocol::S2C_ReactionSkill reactionSkill;
    reactionSkill.set_remote_id(owner->GetGameObjectID());
    reactionSkill.set_object_id(this->GetGameObjectID());
    reactionSkill.set_skill_id(this->GetSkillID());
    reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
    reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
    reactionSkill.set_duration(worldTime);
}

void WarriorDash::SetWarriorDash(float inDistance, float inSpeed)
{
    GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }
    const int64& worldTime = world->GetWorldTime();

    GameRemotePlayerPtr owner = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
    if (nullptr == owner)
    {
        return;
    }

    PlayerCharacterPtr instigated = std::static_pointer_cast<PlayerCharacter>(owner->GetCharacter());
    if (nullptr == instigated)
    {
        return;
    }

    mDashDistance = inDistance;
    mInitDashSpeed = inSpeed;

    StatsComponent& playerStats = instigated->GetStatComponent();
    BuffComponent& playerbuff = instigated->GetBuffComponent();

    mDashSpeed = inSpeed - playerStats.GetCurrentStats().GetMovementSpeed();

    playerbuff.PushBuff(playerStats, EStatType::Stat_MovementSpeed, mDashSpeed);

}
