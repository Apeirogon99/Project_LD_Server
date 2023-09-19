#include "pch.h"
#include "Trigger.h"

Trigger::Trigger(const WCHAR* inName) : Actor(inName), mOverlapType(EActorType::Unspecified), mIsTrigger(false), mMaxOverlap(INFINITE)
{
}

Trigger::~Trigger()
{
}

void Trigger::OnInitialization()
{
    this->SetTick(true, GAME_TICK);
}

void Trigger::OnDestroy()
{
    mOverlapActor.clear();
}

bool Trigger::IsValid()
{
    return mIsTrigger;
}

void Trigger::SetOverlapType(EActorType inOverlapActorType)
{
    mOverlapType = inOverlapActorType;
}

void Trigger::SetTrigger(bool inIsTrigger)
{
    mIsTrigger = inIsTrigger;
}

void Trigger::SetMaxOverlap(uint32 inMaxOverlap)
{
    mMaxOverlap = inMaxOverlap;
}

BoxTrigger::BoxTrigger() : Trigger(L"BoxTrigger")
{
    this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

BoxTrigger::~BoxTrigger()
{
}

void BoxTrigger::OnTick(const int64 inDeltaTime)
{
    if (false == IsValid())
    {
        return;
    }

    GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }

    Location                location = this->GetLocation();
    Rotation                rotation = this->GetRotation();
    BoxCollisionComponent*  component = this->GetBoxCollisionComponent();
    FVector                 extent = component->GetBoxCollision().GetBoxExtent();

    const float debugDuration = 0.05f;
    PacketUtils::DebugDrawBox(this->GetPlayerViewers(), location, location, extent, debugDuration);

    BoxTrace trace(this->GetActorRef(), location, location, true, extent, rotation);

    std::vector<ActorPtr> outActors;
    world->FindActors(trace, static_cast<uint8>(mOverlapType), outActors, mMaxOverlap);

    for (std::pair<const ActorPtr, bool>& overlap : mOverlapActor)
    {
        overlap.second = false;
    }

    for (auto actor : outActors)
    {
        auto findActor = mOverlapActor.find(actor);
        if (findActor == mOverlapActor.end())
        {
            if (actor == nullptr)
            {
                continue;
            }

            this->OnBeginOverlap(actor);
            mOverlapActor.insert(std::pair(actor, true));
        }
        else
        {
            findActor->second = true;
        }
    }

    auto pos = mOverlapActor.begin();
    for (pos; pos != mOverlapActor.end();)
    {
        std::pair<const ActorPtr, bool>& overlap = *pos;
        if (false == overlap.second)
        {
            ActorPtr actor = overlap.first;
            this->OnEndOverlap(actor);
            mOverlapActor.erase(pos++);
        }
        else
        {
            ++pos;
        }
    }

    this->OnOverlapTick(inDeltaTime);

}

BoxCollisionComponent* BoxTrigger::GetBoxCollisionComponent() const
{
    return static_cast<BoxCollisionComponent*>(this->mDefaultCollisionComponent);
}

SphereTrigger::SphereTrigger() : Trigger(L"SphereTrigger")
{
    this->mDefaultCollisionComponent = new SphereCollisionComponent;
}

SphereTrigger::~SphereTrigger()
{
}

void SphereTrigger::OnTick(const int64 inDeltaTime)
{
    if (false == IsValid())
    {
        return;
    }

    GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
    if (nullptr == world)
    {
        return;
    }

    Location                    location = this->GetLocation();
    Rotation                    rotation = this->GetRotation();
    SphereCollisionComponent*   component = this->GetSphereCollisionComponent();
    float                       radius = component->GetSphereCollision().GetRadius();

    const float debugDuration = 0.05f;
    PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), location, radius, debugDuration);

    SphereTrace trace(this->GetActorRef(), location, true, radius);
    std::vector<ActorPtr> outActors;
    world->FindActors(trace, static_cast<uint8>(mOverlapType), outActors, mMaxOverlap);

    for (std::pair<const ActorPtr, bool>& overlap : mOverlapActor)
    {
        overlap.second = false;
    }

    for (auto actor : outActors)
    {
        auto findActor = mOverlapActor.find(actor);
        if (findActor == mOverlapActor.end())
        {
            if (actor == nullptr)
            {
                continue;
            }

            this->OnBeginOverlap(actor);
            mOverlapActor.insert(std::pair(actor, true));
        }
        else
        {
            findActor->second = true;
        }
    }

    auto pos = mOverlapActor.begin();
    for (pos; pos != mOverlapActor.end();)
    {
        std::pair<const ActorPtr, bool>& overlap = *pos;
        if (false == overlap.second)
        {
            ActorPtr actor = overlap.first;
            this->OnEndOverlap(actor);
            mOverlapActor.erase(pos++);
        }
        else 
        {
            ++pos;
        }
    }

    this->OnOverlapTick(inDeltaTime);

}

SphereCollisionComponent* SphereTrigger::GetSphereCollisionComponent() const
{
    return static_cast<SphereCollisionComponent*>(this->mDefaultCollisionComponent);
}
