#include "pch.h"
#include "EnemyNomalSkeleton.h"

EnemyNomalSkeleton::EnemyNomalSkeleton() : EnemyCharacter(L"Enemy::NomalSkeleton")
{
}

EnemyNomalSkeleton::~EnemyNomalSkeleton()
{
}

void EnemyNomalSkeleton::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);
	this->mSpawnParticle = static_cast<int32>(ESkillID::Skill_Rich_Rise_Skeleton);
	this->mSpawnParticleDelay = 3000;

	this->SetEnemyType(EnemyType::Nomal);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos infos;
	infos.push_back(AttackInfo(0, 450, 1700, FVector(50.0f, 80.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Melee, infos);
}

void EnemyNomalSkeleton::OnAutoAttackShot(ActorPtr inVictim)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	{
		Protocol::S2C_EnemyAutoAttack autoAttackPacket;
		autoAttackPacket.set_object_id(this->GetGameObjectID());
		autoAttackPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
		autoAttackPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
		this->BrodcastPlayerViewers(sendBuffer);
	}
}

void EnemyNomalSkeleton::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	ActorPtr actor = world->SpawnActor<EnemyMeleeAttack>(this->GetActorRef(), this->GetLocation(), this->GetRotation(), FVector(1.0f, 1.0f, 1.0f));
	if (nullptr == actor)
	{
		return;
	}

	mMeleeAttack = std::static_pointer_cast<EnemyMeleeAttack>(actor);
	mMeleeAttack->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Melee);
	mMeleeAttack->SetTargetActorType(EActorType::Player);
	mMeleeAttack->SetAttackExtent(inRange);
	mMeleeAttack->SetDamage(inDamage);
	mMeleeAttack->SetParryinglTime(worldTime, worldTime + 200);

	AttackInfo info = mAutoAttackComponent.GetAttackInfo(0);
	mMeleeAttack->PushTask(worldTime + 250, &EnemyMeleeAttack::CheackCollision);
	mMeleeAttack->PushTask(worldTime + 300, &EnemyMeleeAttack::PushReserveDestroy);

}

void EnemyNomalSkeleton::OnAutoAttackOver()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	mMeleeAttack.reset();
	this->mAutoAttackComponent.OnOverAutoAttack();

	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() != EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Chase);
		}
	}
}

void EnemyNomalSkeleton::OnStunWakeUp()
{
	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() == EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Chase);
		}
	}
}

void EnemyNomalSkeleton::OnReward()
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
	money->SetAmount(static_cast<int32>(Random::GetNormalDistribution(70, 0.6)));

	if (true == Random::GetRandomProbability(1.0, 100.0, 60.0))
	{
		AItemPtr bone = std::static_pointer_cast<AItem>(world->SpawnActor<AItem>(world->GetGameObjectRef(), Random::GetRandomVectorInRange2D(location, 100.0f), FRotator(), FVector()));
		bone->SetItemCode(173);
	}

	PlayerCharacterPtr instigated = std::static_pointer_cast<PlayerCharacter>(this->GetAggroActor().lock());
	if (nullptr == instigated)
	{
		return;
	}

	instigated->GetLevelComponent().AddExperience(static_cast<int32>(Random::GetNormalDistribution(40, 0.8)));
}
