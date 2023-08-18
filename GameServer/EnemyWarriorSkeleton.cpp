#include "pch.h"
#include "EnemyWarriorSkeleton.h"

EnemyWarriorSkeleton::EnemyWarriorSkeleton() : EnemyCharacter(L"Enemy::WarriorSkeleton")
{
}

EnemyWarriorSkeleton::~EnemyWarriorSkeleton()
{
}

void EnemyWarriorSkeleton::OnInitialization()
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

	this->mCapsuleCollisionComponent.SetOwner(this->GetActorRef());
	this->mCapsuleCollisionComponent.SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos infos;
	infos.push_back(AttackInfo(0, 600, 1700, FVector(50.0f, 80.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Melee, infos);
}

void EnemyWarriorSkeleton::OnAutoAttackShot(ActorPtr inVictim)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	Protocol::SRotator rotation = PacketUtils::ToSRotator(FRotator(0.0f, this->GetRotation().GetYaw(), 0.0f));

	Protocol::S2C_EnemyAutoAttack autoAttackPacket;
	autoAttackPacket.set_object_id(this->GetGameObjectID());
	autoAttackPacket.mutable_rotation()->CopyFrom(rotation);
	autoAttackPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void EnemyWarriorSkeleton::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	FVector		location = this->mMovementComponent.GetCurrentLocation(this->GetActorPtr());
	FRotator	rotation = FRotator(0.0f, this->GetRotation().GetYaw(), 0.0f);
	FVector		foward = rotation.GetForwardVector();
	const float collision = this->mCapsuleCollisionComponent.GetBoxCollision().GetBoxExtent().GetX();
	const float radius = std::sqrtf(std::powf(inRange.GetX(), 2) + std::powf(inRange.GetY(), 2));	//외접원 반지름

	Location boxStartLocation = location;
	Location boxEndLocation = boxStartLocation + (foward * (inRange.GetX() * 2));
	Location boxCenterLocation = (boxStartLocation + boxEndLocation) / 2.0f;
	BoxTrace boxTrace(boxStartLocation, boxEndLocation, true, inRange, rotation);

	//DEBUG
	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawBox(this->GetPlayerViewers(), boxStartLocation, boxEndLocation, inRange, debugDuration);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), boxCenterLocation, radius, debugDuration);

	auto playerIter = mPlayerViewers.begin();
	for (playerIter; playerIter != mPlayerViewers.end(); ++playerIter)
	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerIter->get()->GetRemotePlayer());
		PlayerCharacterPtr character = remotePlayer->GetCharacter();

		bool isOverlap = boxTrace.BoxCollisionTrace(character->GetCapsuleCollisionComponent());
		if (isOverlap)
		{
			character->PushTask(worldTime, &Actor::OnHit, this->GetActorPtr(), inDamage);
		}

	}
}

void EnemyWarriorSkeleton::OnAutoAttackOver()
{
	this->mAutoAttackComponent.OnOverAutoAttack();

	if (false == this->IsDeath())
	{
		this->mStateManager.SetState(EStateType::State_Chase);
	}
}

void EnemyWarriorSkeleton::OnReward()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	float halfHeight = this->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetZ() / 2.0f;
	Location location = FVector(this->GetLocation().GetX(), this->GetLocation().GetY(), this->GetLocation().GetZ() - halfHeight);

	AItemPtr money = std::static_pointer_cast<AItem>(world->SpawnActor<AItem>(world->GetGameObjectRef(), Random::GetRandomVectorInRange2D(location, 100.0f), FRotator(), FVector()));
	money->SetItemCode(171);
	money->SetAmount(static_cast<int32>(Random::GetNormalDistribution(130, 0.8)));

	if (true == Random::GetRandomProbability(1.0, 100.0, 20.0))
	{
		AItemPtr bone = std::static_pointer_cast<AItem>(world->SpawnActor<AItem>(world->GetGameObjectRef(), Random::GetRandomVectorInRange2D(location, 100.0f), FRotator(), FVector()));
		bone->SetItemCode(173);
	}

	PlayerCharacterPtr instigated = std::static_pointer_cast<PlayerCharacter>(this->GetAggroActor().lock());
	if (nullptr == instigated)
	{
		return;
	}

	instigated->GetLevelComponent().AddExperience(static_cast<int32>(Random::GetNormalDistribution(100, 0.8)));
}
