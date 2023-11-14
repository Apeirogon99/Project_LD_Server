#include "pch.h"
#include "AttackTestUnit.h"
#include "AttackTestUnitSpanwer.h"

AttackTestUnit::AttackTestUnit() : Actor(L"TestOBB"), mOverlap(false), mTimeStamp(L"Search"), mSyncTime(100)
{
	this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

AttackTestUnit::~AttackTestUnit()
{
}

ProjectileComponent& AttackTestUnit::GetProjectileComponent()
{
	return mProjectileComponent;
}

BoxCollisionComponent* AttackTestUnit::GetBoxCollisionComponent() const
{
	return static_cast<BoxCollisionComponent*>(this->mDefaultCollisionComponent);
}

void AttackTestUnit::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);
	this->SetActorType(static_cast<uint8>(EActorType::EnemyAttack));

	this->mProjectileComponent.InitProjectile(GAME_TICK, world->GetWorldTime());

	const float velocity = static_cast<float>(Random::GetRealUniformDistribution(100, 200));
	this->SetVelocity(velocity, velocity, velocity);

	FVector boxExtent;
	boxExtent.SetX(static_cast<float>(Random::GetRealUniformDistribution(50, 100)));
	boxExtent.SetY(static_cast<float>(Random::GetRealUniformDistribution(50, 100)));
	boxExtent.SetZ(static_cast<float>(Random::GetRealUniformDistribution(50, 100)));

	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
	collision->SetBoxCollision(boxExtent);
	collision->SetOwner(this->GetActorRef());
}

void AttackTestUnit::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void AttackTestUnit::OnTick(const int64 inDeltaTime)
{

	mSyncTime -= inDeltaTime;
	if (mSyncTime > 0)
	{
		return;
	}
	mSyncTime = Random::GetIntUniformDistribution(10, 100);

	this->TickUnitCollision();
	this->TickWallCollision();

	this->mProjectileComponent.Update(this->GetActorPtr());
	//this->SyncLocation(inDeltaTime);
}

bool AttackTestUnit::IsValid()
{
	return true;
}

void AttackTestUnit::OnAppearActor(ActorPtr inAppearActor)
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

	Protocol::S2C_ApeearAttackTestUnit appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.mutable_extent()->CopyFrom(PacketUtils::ToSVector(this->GetBoxCollisionComponent()->GetBoxCollision().GetBoxExtent()));
	appearPacket.mutable_velocity()->CopyFrom(PacketUtils::ToSVector(this->GetVelocity()));
	appearPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->mProjectileComponent.GetCurrentLocation(this->GetActorPtr())));
	appearPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearPacket.set_timestamp(this->GetWorld().lock()->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearPacket);
	anotherPlayerState->Send(sendBuffer);
}

void AttackTestUnit::OnDisAppearActor(ActorPtr inDisappearActor)
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

void AttackTestUnit::TickWallCollision()
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
	std::shared_ptr<AttackTestUnitSpanwer> spawner = std::static_pointer_cast<AttackTestUnitSpanwer>(owner);

	const Location& spawnerLocation = spawner->GetLocation();
	const float maxGroundRange		= spawner->mMaxGroundRange;

	FVector		location = this->mProjectileComponent.GetCurrentLocation(this->GetActorPtr());
	FRotator	rotation = this->GetRotation();
	const int64	lastMovement = this->GetWorld().lock()->GetWorldTime();

	FRotator newRotation = FRotator(0.0f, 0.0f, 0.0f);
	if (spawnerLocation.GetX() + maxGroundRange <= location.GetX())
	{
		newRotation = FRotator(0.0f, static_cast<float>(fmod(Random::GetRealUniformDistribution(180, 360), 180)), 0.0f);
	}
	else if (spawnerLocation.GetX() - maxGroundRange >= location.GetX())
	{
		newRotation = FRotator(0.0f, static_cast<float>(Random::GetRealUniformDistribution(0, 180)), 0.0f);
	}
	else if (spawnerLocation.GetY() + maxGroundRange <= location.GetY())
	{
		newRotation = FRotator(0.0f, static_cast<float>(Random::GetRealUniformDistribution(-90, 90)), 0.0f);
	}
	else if (spawnerLocation.GetY() - maxGroundRange >= location.GetY())
	{
		newRotation = FRotator(0.0f, static_cast<float>(fmod(Random::GetRealUniformDistribution(90, 270), 180)), 0.0f);
	}
	else
	{
		return;
	}

	spawner->MakeMovementPacket(this->GetGameObjectID(), location, newRotation, lastMovement);
	this->SetRotation(newRotation);
}

void AttackTestUnit::TickUnitCollision()
{
	mTimeStamp.StartTimeStamp();

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	FVector		location	= this->GetLocation();
	FRotator	rotation	= this->GetRotation();
	FVector		foward		= rotation.GetForwardVector();

	std::vector<ActorPtr> findActors;
	FVector		boxExtent			= this->GetBoxCollisionComponent()->GetBoxCollision().GetBoxExtent();
	const float radius				= std::sqrtf(std::powf(boxExtent.GetX(), 2) + std::powf(boxExtent.GetY(), 2));
	Location	boxStartLocation	= location;
	Location	boxEndLocation		= boxStartLocation + (foward * (boxExtent.GetX() * 2));
	Location	boxCenterLocation	= (boxStartLocation + boxEndLocation) / 2.0f;
	
	BoxTrace	boxTrace(this->GetActorRef(), boxStartLocation, boxEndLocation, true, boxExtent, rotation);
	SphereTrace	sphereTrace(this->GetActorRef(), location, true, radius);

	uint8 findActorType = static_cast<uint8>(EActorType::EnemyAttack);
	//uint8 findActorType = static_cast<uint8>(EActorType::Player);

	bool result = world->FindActors(sphereTrace, findActorType, findActors);
	const int64 searchTime = mTimeStamp.GetTimeStamp();

	//if (findActors.size() > 30)
	//{
	//	this->GameObjectLog(L"UnitCollision Result : SearchTime [%lld ms], FindActor [%lld]\n", searchTime, findActors.size());
	//}
	
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), location, radius, 0.05f);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	//if (nullptr == owner)
	//{
	//	return;
	//}
	//std::shared_ptr<AttackTestUnitSpanwer> spawner = std::static_pointer_cast<AttackTestUnitSpanwer>(owner);

	//if (!result)
	//{
	//	if (false == mOverlap)
	//	{
	//		return;
	//	}
	//	mOverlap = false;

	//	spawner->MakeEndOverlapPacket(this->GetGameObjectID());

	//}
	//else
	//{
	//	if (true == mOverlap)
	//	{
	//		return;
	//	}
	//	mOverlap = true;

	//	spawner->MakeBeginOverlapPacket(this->GetGameObjectID());
	//}

}
