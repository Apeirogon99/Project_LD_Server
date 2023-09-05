#include "pch.h"
#include "Arrow.h"

Arrow::Arrow() : EnemyAttack(L"EnemyAttack::Arrow"), mIsCollision(false)
{
	this->mDefaultCollisionComponent = new SphereCollisionComponent;
}

Arrow::~Arrow()
{

}

void Arrow::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	SphereCollisionComponent* collision = GetSphereCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetSphereCollisione(20.0f);

	this->mProjectileComponent.InitProjectile(GAME_TICK, world->GetWorldTime());

	this->SetVelocity(1000.0f, 1000.0f, 1000.0f);

	this->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Projectile);
}

void Arrow::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void Arrow::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	if (this->IsLife(inDeltaTime))
	{
		GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
		if (nullptr == world)
		{
			return;
		}

		world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
		return;
	}

	this->mProjectileComponent.Update(this->GetActorPtr());
	this->SyncLocation(inDeltaTime);
	this->CheackCollision();

}

bool Arrow::IsValid()
{

	if (true == mIsCollision)
	{
		return false;
	}

	return true;
}

void Arrow::OnAppearActor(ActorPtr inAppearActor)
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

	FVector		location = this->mProjectileComponent.GetCurrentLocation(this->GetActorPtr());
	FRotator	rotation = this->GetRotation();
	const int64	lastMovement = this->GetWorld().lock()->GetWorldTime();

	Protocol::S2C_AppearArrow appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(location));
	appearPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(rotation));
	appearPacket.set_timestamp(lastMovement);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearPacket);
	anotherPlayerState->Send(sendBuffer);
}

void Arrow::OnDisAppearActor(ActorPtr inDisappearActor)
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

void Arrow::SyncLocation(const int64 inDeltaTime)
{

	bool isSync = this->mProjectileComponent.SyncUpdate(this->GetActorPtr(), inDeltaTime);
	if (false == isSync)
	{
		return;
	}

	this->OnMovement();

}

void Arrow::OnMovement()
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

void Arrow::CheackCollision()
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
	FVector		location	= this->mProjectileComponent.GetCurrentLocation(this->GetActorPtr());
	const float radius		= collision->GetSphereCollision().GetRadius();
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

void Arrow::OnParrying(ActorPtr inActor)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	ActorPtr actor = world->SpawnActor<Arrow>(inActor->GetGameObjectRef(), this->GetLocation(), FRotator::TurnRotator(this->GetRotation()), Scale(1.0f, 1.0f, 1.0f));
	ArrowPtr arrow = std::static_pointer_cast<Arrow>(actor);
	if (nullptr == arrow)
	{
		return;
	}

	arrow->SetDamage(this->GetDamage());
	arrow->ReserveDestroy(this->GetMaxLifeTime());
	arrow->SetTargetActorType(this->GetTargetActorType());

	bool ret = world->DestroyActor(this->GetGameObjectID());
	if (false == ret)
	{
		this->GameObjectLog(L"Can't destroy parrying\n");
	}
}

ProjectileComponent& Arrow::GetProjectileComponent()
{
	return mProjectileComponent;
}

SphereCollisionComponent* Arrow::GetSphereCollisionComponent()
{
	return static_cast<SphereCollisionComponent*>(this->GetDefaultCollisionComponent());
}


