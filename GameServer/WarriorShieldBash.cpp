#include "pch.h"
#include "WarriorShieldBash.h"

WarriorShieldBash::WarriorShieldBash() : ActiveSkill(L"WarriorShieldBash"), mSturnRadis(0.0f), mMaxRadius(0.0f), mDamage(0.0f), mDebuffMovement(0.0f), mSturnDuration(0), mSlowDuration(0), mActiveSlowTime(0)
{
}

WarriorShieldBash::~WarriorShieldBash()
{
}

void WarriorShieldBash::OnInitialization()
{
}

void WarriorShieldBash::OnDestroy()
{
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
	return true;
}

void WarriorShieldBash::SetWarriorShieldBash(const float& inSturnRadius, const float& inMaxRadius, const float inDamage, const float inDebuffMovement, const int64& inSturnDuration, const int64& inSlowDuration, const int64& inActiveSlowTime)
{
    this->mSturnRadis       = inSturnRadius;
    this->mMaxRadius        = inMaxRadius;
    this->mDamage           = inDamage;
    this->mDebuffMovement   = inDebuffMovement;
    this->mSturnDuration    = inSturnDuration;
    this->mSlowDuration     = inSlowDuration;
    this->mActiveSlowTime   = inActiveSlowTime;
}

void WarriorShieldBash::Active()
{

    GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }
    const int64 worldTime = world->GetWorldTime();

    this->PushTask(worldTime, &WarriorShieldBash::SturnActive);
    this->PushTask(worldTime + this->mActiveSlowTime, &WarriorShieldBash::SlowActive);

}

void WarriorShieldBash::SturnActive()
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

    uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
    std::vector<ActorPtr> findActors;
    bool result = world->FindActors(location, mSturnRadis, findActorType, findActors);
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
        float distanceDamage = mDamage * std::lerp(mMaxRadius, 0.0f, distance2D);
        if (distance2D <= mSturnRadis)
        {
            enemy->GetStateManager().SetState(EStateType::State_Stun);

            StunState* stunState = static_cast<StunState*>(enemy->GetStateManager().GetCurrentStateEvent());
            if (nullptr != stunState)
            {
                stunState->SetStunTime(mSturnDuration);
            }

            enemy->PushTask(worldTime, &Actor::OnHit, instigated->GetActorPtr(), distanceDamage);

        }

    }
}

void WarriorShieldBash::SlowActive()
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

    uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
    std::vector<ActorPtr> findActors;
    bool result = world->FindActors(location, mMaxRadius, findActorType, findActors);
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
        float distanceDamage = mDamage * std::lerp(mMaxRadius, 0.0f, distance2D);
        if (mSturnRadis < distance2D && distance2D <= mMaxRadius)
        {
            enemy->PushTask(worldTime + 0, &EnemyCharacter::OnBuffChanage, EStatType::Stat_MovementSpeed, mDebuffMovement, true);
            enemy->PushTask(worldTime + mSlowDuration, &EnemyCharacter::OnBuffChanage, EStatType::Stat_MovementSpeed, mDebuffMovement, false);

            enemy->PushTask(worldTime, &Actor::OnHit, instigated->GetActorPtr(), distanceDamage);
        }

    }
}
