#include "pch.h"
#include "SourSpear.h"

SourSpear::SourSpear() : EnemyAttack(L"SourSpear"), mIsCollision(false), mStartTime(0), mIsActive(false)
{
	this->mDefaultCollisionComponent = new SphereCollisionComponent;
}

SourSpear::~SourSpear()
{
}

void SourSpear::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}
	mStartTime = world->GetWorldTime();

	SetTick(true, SYSTEM_TICK);

	SphereCollisionComponent* collision = GetSphereCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetSphereCollisione(20.0f);

	this->mProjectileComponent.InitProjectile(GAME_TICK, world->GetWorldTime());

	this->SetVelocity(1500.0f, 1500.0f, 1500.0f);
	this->SetDamage(100.0f);
	this->ReserveDestroy(1000);

	this->PushTask(mStartTime + 200, &SourSpear::Active);
}

void SourSpear::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void SourSpear::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	mCurrentLifeTime += inDeltaTime;
	if (mCurrentLifeTime >= mMaxLifeTime)
	{
		GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
		if (nullptr == world)
		{
			return;
		}

		bool ret = world->DestroyActor(this->GetGameObjectID());
		if (false == ret)
		{
			this->GameObjectLog(L"Can't destroy arrow\n");
		}

		return;
	}

	if (this->mIsActive)
	{
		this->mProjectileComponent.Update(this->GetActorPtr());
		this->CheackCollision();
	}

	this->SyncLocation(inDeltaTime);
}

bool SourSpear::IsValid()
{
	if (true == mIsCollision)
	{
		return false;
	}

	return true;
}

void SourSpear::OnAppearActor(ActorPtr inAppearActor)
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
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Soul_Spear));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime - this->mStartTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void SourSpear::OnDisAppearActor(ActorPtr inDisappearActor)
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

void SourSpear::CheackCollision()
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

	SphereCollisionComponent* collision = GetSphereCollisionComponent();
	FVector		location = this->mProjectileComponent.GetCurrentLocation(this->GetActorPtr());
	const float radius = collision->GetSphereCollision().GetRadius();
	SphereTrace	sphereTrace(this->GetActorRef(), location, true, radius);

	const float debugDuration = 0.1f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), location, radius, debugDuration);

	uint8 findActorType = static_cast<uint8>(this->mTargetActorType);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(sphereTrace, findActorType, findActors, 1);
	if (!result)
	{
		return;
	}

	for (ActorPtr actor : findActors)
	{

		actor->PushTask(worldTime, &Actor::OnHit, owner, this->GetDamage());

		bool ret = world->DestroyActor(this->GetGameObjectID());
		if (false == ret)
		{
			this->GameObjectLog(L"Can't destroy arrow\n");
		}
		mIsCollision = true;
		break;
	}
}

void SourSpear::OnParrying(ActorPtr inActor)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	ActorPtr actor = world->SpawnActor<SourSpear>(inActor->GetGameObjectRef(), this->GetLocation(), FRotator::TurnRotator(this->GetRotation()), Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<SourSpear> sourSpear = std::static_pointer_cast<SourSpear>(actor);
	if (nullptr == sourSpear)
	{
		return;
	}

	sourSpear->SetDamage(this->GetDamage());
	sourSpear->ReserveDestroy(this->GetMaxLifeTime());
	sourSpear->SetTargetActorType(this->GetTargetActorType());

	bool ret = world->DestroyActor(this->GetGameObjectID());
	if (false == ret)
	{
		this->GameObjectLog(L"Can't destroy parrying\n");
	}
}

void SourSpear::OnMovement()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	FVector		location = this->mProjectileComponent.GetCurrentLocation(this->GetActorPtr());
	FRotator	rotation = this->GetRotation();
	const int64	lastMovement = this->GetWorld().lock()->GetWorldTime();

	Protocol::S2C_MovementProjectile movementPacket;
	movementPacket.set_object_id(this->GetGameObjectID());
	movementPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(location));
	movementPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(rotation));
	movementPacket.set_timestamp(lastMovement);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, movementPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void SourSpear::Active()
{
	mIsActive = true;

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	Protocol::S2C_ReactionSkill reactionSkill;
	reactionSkill.set_remote_id(owner->GetGameObjectID());
	reactionSkill.set_object_id(this->GetGameObjectID());
	reactionSkill.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Soul_Spear));
	reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	reactionSkill.set_duration(worldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
	this->BrodcastPlayerViewers(sendBuffer);

	world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
	return;
}

void SourSpear::SyncLocation(const int64 inDeltaTime)
{
	bool isSync = this->mProjectileComponent.SyncUpdate(this->GetActorPtr(), inDeltaTime);
	if (false == isSync)
	{
		return;
	}

	this->OnMovement();

}

ProjectileComponent& SourSpear::GetProjectileComponent()
{
	return mProjectileComponent;
}

SphereCollisionComponent* SourSpear::GetSphereCollisionComponent()
{
	return static_cast<SphereCollisionComponent*>(this->GetDefaultCollisionComponent());
}