#include "pch.h"
#include "Arrow.h"

Arrow::Arrow() : Actor(L"Arrow"), mIsCollision(false), mDamage(0.0f), mLifeTime(0)
{
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

	this->mCollisionComponent.SetOwner(this->GetActorRef());
	this->mCollisionComponent.SetSphereCollisione(20.0f);

	this->mProjectileComponent.InitProjectile(this->GetLocation(), this->GetRotation(), GAME_TICK, world->GetWorldTime());

	this->SetVelocity(1000.0f, 1000.0f, 1000.0f);
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

	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	mLifeTime -= inDeltaTime;
	if (mLifeTime <= 0)
	{
		bool ret = world->DestroyActor(this->GetGameObjectID());
		if (false == ret)
		{
			this->GameObjectLog(L"Can't destroy arrow\n");
		}
		return;
	}

	this->mProjectileComponent.Update(this->GetActorPtr());
	this->SyncLocation(inDeltaTime);
	this->CheackCollision();

}

bool Arrow::IsValid()
{
	return mLifeTime != 0 || mIsCollision == true;
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
	const int64	worldTime = this->GetWorld().lock()->GetWorldTime();


	Protocol::S2C_AppearArrow appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(location));
	appearPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(rotation));
	appearPacket.set_timestamp(this->mProjectileComponent.GetLastMovementTime());

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
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	bool isSync = this->mProjectileComponent.SyncUpdate(this->GetActorPtr(), inDeltaTime);
	if (false == isSync)
	{
		return;
	}
	Protocol::SVector	currentLocation = PacketUtils::ToSVector(this->GetLocation());
	const int64			worldTime = this->mProjectileComponent.GetLastMovementTime();


	Protocol::S2C_MovementProjectile movementPacket;
	movementPacket.set_object_id(this->GetGameObjectID());
	movementPacket.mutable_location()->CopyFrom(currentLocation);
	movementPacket.set_timestamp(worldTime);

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

	FVector		location	= this->mProjectileComponent.GetCurrentLocation(this->GetActorPtr());
	FVector		foward		= this->GetRotation().GetForwardVector() * -20.0f;
	FVector		arrowHead	= location + foward;
	const float radius		= this->mCollisionComponent.GetSphereCollision().GetRadius();
	SphereTrace	sphereTrace(arrowHead, arrowHead, true, radius);

	uint8 findActorType = static_cast<uint8>(EActorType::Player);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(arrowHead, 100.0f, findActorType, findActors);
	if (!result)
	{
		return;
	}

	for (ActorPtr actor : findActors)
	{
		PlayerCharacterPtr player = std::static_pointer_cast<PlayerCharacter>(actor);
		if (nullptr == player)
		{
			continue;
		}

		bool isOverlap = sphereTrace.BoxCollisionTrace(player->GetCapsuleCollisionComponent());
		if (isOverlap)
		{
			player->PushTask(worldTime, &Actor::OnHit, this->GetActorPtr(), this->mDamage);

			bool ret = world->DestroyActor(this->GetGameObjectID());
			if (false == ret)
			{
				this->GameObjectLog(L"Can't destroy arrow\n");
			}
			mIsCollision = true;
			break;
		}

	}
}

void Arrow::SetDamage(const float& inDamage)
{
	mDamage = inDamage;
}

void Arrow::SetLifeTime(const int64& inLifeTime)
{
	mLifeTime = inLifeTime;
}

const float Arrow::GetDamage() const
{
	return mDamage;
}

const int64 Arrow::GetLifeTime() const
{
	return mLifeTime;
}

ProjectileComponent& Arrow::GetProjectileComponent()
{
	return mProjectileComponent;
}

SphereCollisionComponent& Arrow::GetSphereCollisionComponent()
{
	return mCollisionComponent;
}


