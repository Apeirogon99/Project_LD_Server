#include "pch.h"
#include "Extermination.h"

Extermination::Extermination() : EnemyAttack(L"Extermination"), mStartSphereSize(1.0f)
{
	this->mDefaultCollisionComponent = new SphereCollisionComponent;
}

Extermination::~Extermination()
{
}

void Extermination::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	SphereCollisionComponent* collision = GetSphereCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetSphereCollisione(mStartSphereSize);

	this->SetDamage(9999.0f);
	this->SetActorType(static_cast<uint8>(EActorType::EnemyAttack));
	this->SetTargetActorType(EActorType::Player);
	this->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Hard_Place);
}

void Extermination::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void Extermination::OnTick(const int64 inDeltaTime)
{

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	this->PushTask(world->GetNextWorldTime(), &Extermination::CheackCollision);
}

bool Extermination::IsValid()
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
			world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
			return false;
		}

		//world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
	}

	return true;
}

void Extermination::OnAppearActor(ActorPtr inAppearActor)
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
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Extermination));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void Extermination::OnDisAppearActor(ActorPtr inDisappearActor)
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

void Extermination::CheackCollision()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	if (!owner->IsValid())
	{
		return;
	}

	SphereCollisionComponent*	collision = GetSphereCollisionComponent();
	FVector						location = this->GetLocation();
	SphereTrace					sphereTrace(this->GetActorRef(), location, true, mStartSphereSize);

	const float debugDuration = 0.05f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), location, mStartSphereSize, debugDuration);

	Protocol::S2C_ReactionSkill reactionSkill;
	reactionSkill.set_remote_id(owner->GetGameObjectID());
	reactionSkill.set_object_id(this->GetGameObjectID());
	reactionSkill.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Extermination));
	reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	reactionSkill.set_duration(static_cast<int64>(mStartSphereSize));

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
	this->BrodcastPlayerViewers(sendBuffer);

	mStartSphereSize += 50.0f;

	uint8 findActorType = static_cast<uint8>(this->mTargetActorType);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(sphereTrace, findActorType, findActors);
	if (!result)
	{
		return;
	}

	for (ActorPtr actor : findActors)
	{
		if (actor->IsValid())
		{
			actor->PushTask(worldTime, &Actor::OnHit, owner, this->GetDamage());
		}
	}
}

bool Extermination::OnParrying(ActorPtr inActor)
{
	return false;
}

SphereCollisionComponent* Extermination::GetSphereCollisionComponent()
{
	return static_cast<SphereCollisionComponent*>(this->GetDefaultCollisionComponent());
}
