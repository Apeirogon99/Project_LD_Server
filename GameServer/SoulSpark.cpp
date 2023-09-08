#include "pch.h"
#include "SoulSpark.h"

SoulSpark::SoulSpark() : EnemyAttack(L"SoulSpark"), mStartTime(0), mEndTime(10000), mTargetTrace(nullptr)
{
	this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

SoulSpark::~SoulSpark()
{
	if (this->mTargetTrace)
	{
		delete this->mTargetTrace;
	}
	this->mTargetTrace = nullptr;
}

void SoulSpark::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	mStartTime = world->GetWorldTime();

	this->PushTask(mStartTime + 500, &SoulSpark::CheackTargeting);
	this->PushTask(mStartTime + 1100, &SoulSpark::CheackCollision);
}

void SoulSpark::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void SoulSpark::OnTick(const int64 inDeltaTime)
{
}

bool SoulSpark::IsValid()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
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

void SoulSpark::OnAppearActor(ActorPtr inAppearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inAppearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == IsValid())
	{
		return;
	}

	if (mPlayerViewers.find(anotherPlayerState) != mPlayerViewers.end())
	{
		return;
	}
	this->InsertPlayerViewer(anotherPlayerState);
	anotherPlayerState->InsertActorMonitor(this->GetActorPtr());

	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(owner->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Soul_Spark));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime - this->mStartTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void SoulSpark::OnDisAppearActor(ActorPtr inDisappearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inDisappearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == this->FindPlayerViewer(anotherPlayerState))
	{
		return;
	}
	this->ReleasePlayerViewer(anotherPlayerState);
	anotherPlayerState->ReleaseActorMonitor(this->GetActorPtr());

	Protocol::S2C_DisAppearGameObject disAppearItemPacket;
	disAppearItemPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disAppearItemPacket);
	anotherPlayerState->Send(appearItemSendBuffer);
}

void SoulSpark::CheackTargeting()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	const int64& duration = mStartTime + 3000 - worldTime;

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	if (nullptr == mTarget)
	{
		if (false == this->SearchTarget())
		{
			return;
		}
	}

	FVector boxExtent(850.0f, 100.0f, 100.0f);

	FVector		location = this->GetLocation();
	FVector		targetLocation = FVector(mTarget->GetLocation().GetX(), mTarget->GetLocation().GetY(), location.GetZ());

	FRotator	rotation = (targetLocation - location).Rotator();
	FVector		foward = rotation.GetForwardVector();
	const float radius = std::sqrtf(std::powf(boxExtent.GetX(), 2) + std::powf(boxExtent.GetY(), 2));	//외접원 반지름

	Location boxStartLocation = location;
	Location boxEndLocation = boxStartLocation + (foward * (boxExtent.GetX() * 2));
	Location boxCenterLocation = (boxStartLocation + boxEndLocation) / 2.0f;
	mTargetTrace = new BoxTrace(owner, boxStartLocation, boxEndLocation, true, boxExtent, rotation);
}

void SoulSpark::CheackCollision()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	const int64& duration = mStartTime + 3000 - worldTime;

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	if (nullptr == mTarget)
	{
		if (false == this->SearchTarget())
		{
			return;
		}
	}

	//DEBUG
	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawBox(this->GetPlayerViewers(), mTargetTrace->GetStartLocation(), mTargetTrace->GetEndLocation(), mTargetTrace->GetBoxCollision().GetBoxExtent(), debugDuration);

	Protocol::S2C_ReactionSkill reactionSkill;
	reactionSkill.set_remote_id(owner->GetGameObjectID());
	reactionSkill.set_object_id(this->GetGameObjectID());
	reactionSkill.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Soul_Spark));
	reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(mTargetTrace->GetStartLocation()));
	reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(mTargetTrace->GetBoxCollision().GetOrientation()));
	reactionSkill.set_duration(duration);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
	this->BrodcastPlayerViewers(sendBuffer);

	std::vector<ActorPtr> findActors;
	uint8 findActorType = static_cast<uint8>(EActorType::Player);
	bool result = world->FindActors(*mTargetTrace, findActorType, findActors);
	for (ActorPtr actor : findActors)
	{
		if (actor)
		{
			actor->PushTask(worldTime, &Actor::OnHit, owner, 100.0f);
		}
	}

	bool ret = world->DestroyActor(this->GetGameObjectID());
	if (false == ret)
	{
		this->GameObjectLog(L"Can't destroy skill\n");
	}
}

void SoulSpark::OnParrying(ActorPtr inActor)
{
}

bool SoulSpark::SearchTarget()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}
	const int64 worldTime = world->GetWorldTime();

	std::vector<ActorPtr> targetActors;
	bool result = world->FindActors(this->GetLocation(), 2000.0f, static_cast<uint8>(EActorType::Player), targetActors, 1);
	if (true == result)
	{
		mTarget = targetActors.at(0);
	}

	return result;
}

BoxCollisionComponent* SoulSpark::GetBoxCollisionComponent()
{
	return static_cast<BoxCollisionComponent*>(this->GetDefaultCollisionComponent());
}
