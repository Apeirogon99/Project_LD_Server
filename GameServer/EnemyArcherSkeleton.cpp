#include "pch.h"
#include "EnemyArcherSkeleton.h"

EnemyArcherSkeleton::EnemyArcherSkeleton() : EnemyCharacter(L"Enemy::ArcherSkeleton")
{
}

EnemyArcherSkeleton::~EnemyArcherSkeleton()
{
}

void EnemyArcherSkeleton::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos infos;
	infos.push_back(AttackInfo(0, 550, 1700, FVector(20.0f, 20.0f, 20.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Ranged, infos);
}

void EnemyArcherSkeleton::OnAutoAttackShot(ActorPtr inVictim)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	FVector		instigatedLocation = this->GetLocation();
	FVector		victimLocation = inVictim->GetLocation();
	FVector		direction = victimLocation - instigatedLocation;
	FRotator	rotation = direction.Rotator();

	this->SetRotation(rotation);

	this->GetRotation().ToString();

	{
		Protocol::S2C_EnemyAutoAttack autoAttackPacket;
		autoAttackPacket.set_object_id(this->GetGameObjectID());
		autoAttackPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(FRotator(0.0f, this->GetRotation().GetYaw(), 0.0f)));
		autoAttackPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
		this->BrodcastPlayerViewers(sendBuffer);
	}
}

void EnemyArcherSkeleton::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	Rotation arrowRotation = this->GetRotation();

	Location foward = arrowRotation.Vector();
	foward.SetX(foward.GetX() * 70.0f);
	foward.SetY(foward.GetY() * 70.0f);

	Location arrowLocation = foward + this->GetLocation();
	arrowLocation = Location(arrowLocation.GetX(), arrowLocation.GetY(), arrowLocation.GetZ() + 25.0f);

	//Arrow
	ActorPtr actor = world->SpawnActor<Arrow>(this->GetGameObjectRef(), arrowLocation, arrowRotation, Scale());
	ArrowPtr arrow = std::static_pointer_cast<Arrow>(actor);
	if (nullptr == arrow)
	{
		return;
	}

	//DEBUG
	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), this->GetLocation(), this->GetEnemyStatsComponent().GetCurrentStats().GetRange(), debugDuration);


	arrow->SetDamage(inDamage);
	arrow->ReserveDestroy(5000);
	arrow->SetTargetActorType(EActorType::Player);
}

void EnemyArcherSkeleton::OnAutoAttackOver()
{
	this->mAutoAttackComponent.OnOverAutoAttack();

	if (false == this->IsDeath())
	{
		this->mStateManager.SetState(EStateType::State_Chase);
	}
}

void EnemyArcherSkeleton::OnReward()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	float halfHeight = this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ() / 2.0f;
	Location location = FVector(this->GetLocation().GetX(), this->GetLocation().GetY(), this->GetLocation().GetZ() - halfHeight);

	AItemPtr money = std::static_pointer_cast<AItem>(world->SpawnActor<AItem>(world->GetGameObjectRef(), Random::GetRandomVectorInRange2D(location, 100.0f), FRotator(), FVector()));
	money->SetItemCode(171);
	money->SetAmount(static_cast<int32>(Random::GetNormalDistribution(100, 0.6)));

	if (true == Random::GetRandomProbability(1.0, 100.0, 40.0))
	{
		AItemPtr bone = std::static_pointer_cast<AItem>(world->SpawnActor<AItem>(world->GetGameObjectRef(), Random::GetRandomVectorInRange2D(location, 100.0f), FRotator(), FVector()));
		bone->SetItemCode(173);
	}

	PlayerCharacterPtr instigated = std::static_pointer_cast<PlayerCharacter>(this->GetAggroActor().lock());
	if (nullptr == instigated)
	{
		return;
	}

	instigated->GetLevelComponent().AddExperience(static_cast<int32>(Random::GetNormalDistribution(60, 0.8)));
}
