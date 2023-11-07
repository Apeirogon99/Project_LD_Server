#include "pch.h"
#include "WarriorShieldBash.h"

WarriorShieldBash::WarriorShieldBash() : ActiveSkill(L"WarriorShieldBash"), mSturnRadius(200.0f), mSlowRadius(300.0f), mDamage(0.0f), mDebuffMovement(0.0f), mSturnDuration(3000), mSlowDuration(5000), mActiveTime(2000)
{
}

WarriorShieldBash::~WarriorShieldBash()
{
}

void WarriorShieldBash::OnInitialization()
{
    GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }

    this->BeginCastingSkill();
    this->PushTask(world->GetWorldTime() + 2800, &ActiveSkill::EndCastingSkill);
}

void WarriorShieldBash::OnDestroy()
{

    this->EndCastingSkill();

    Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
    disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

    SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
    this->BrodcastPlayerViewers(sendBuffer);
}

void WarriorShieldBash::OnTick(const int64 inDeltaTime)
{
}

bool WarriorShieldBash::IsValid()
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

void WarriorShieldBash::SetWarriorShieldBash(const float inDamage, const float inDebuffMovement)
{
    this->mDamage           = inDamage;
    this->mDebuffMovement   = inDebuffMovement;
}

void WarriorShieldBash::Active()
{

    GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }
    const int64 worldTime = world->GetWorldTime();

    this->PushTask(worldTime + this->mActiveTime, &WarriorShieldBash::SkillActive);
}

void WarriorShieldBash::SkillActive()
{
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

    GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }
    const int64 worldTime = world->GetWorldTime();

    FVector	location = this->GetLocation();
    SphereTrace	sphereTrace(this->GetActorRef(), location, true, mSlowRadius);

    //DEBUG
    const float duration = 3.0f;
    PacketUtils::DebugDrawSphere(owner->GetRemoteClient().lock(), location, mSturnRadius, duration);
    PacketUtils::DebugDrawSphere(owner->GetRemoteClient().lock(), location, mSlowRadius, duration);

    uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
    std::vector<ActorPtr> findActors;
    bool result = world->FindActors(sphereTrace, findActorType, findActors);
    if (!result)
    {
        return;
    }

    for (ActorPtr actor : findActors)
    {
        EnemyCharacterPtr enemy = std::static_pointer_cast<EnemyCharacter>(actor);
        if (nullptr == enemy)
        {
            continue;
        }

        float distance2D = FVector::Distance2D(location, enemy->GetLocation());
        float distanceDamage = mDamage * (1.0f - (distance2D / mSlowRadius));

        if (enemy->GetEnemyType() == EnemyType::Nomal)
        {
            if (distance2D <= mSturnRadius)
            {
                enemy->GetStateManager().SetState(EStateType::State_Stun);

                StunState* stunState = static_cast<StunState*>(enemy->GetStateManager().GetCurrentStateEvent());
                if (nullptr != stunState)
                {
                    stunState->SetStunTime(mSturnDuration);
                }
            }
            else if (distance2D <= mSlowRadius)
            {
                enemy->PushTask(worldTime + 0, &EnemyCharacter::OnBuffChanage, EBuffType::DeBuff_WarriorShiedBash, EStatType::Stat_MovementSpeed, mDebuffMovement, true);
                enemy->PushTask(worldTime + mSlowDuration, &EnemyCharacter::OnBuffChanage, EBuffType::DeBuff_WarriorShiedBash, EStatType::Stat_MovementSpeed, mDebuffMovement, false);
            }
        }

        enemy->PushTask(worldTime, &Actor::OnHit, instigated->GetActorPtr(), distanceDamage);
    }
}