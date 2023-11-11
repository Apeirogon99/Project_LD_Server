#include "pch.h"
#include "EnemySlime.h"

EnemySlime::EnemySlime() : EnemyCharacter(L"Enemy::Slime")
{
}

EnemySlime::~EnemySlime()
{
}

void EnemySlime::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	this->SetEnemyType(EnemyType::Nomal);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(62.0f, 62.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	Location initLocation = this->GetLocation();
	this->mMovementComponent.SetNewDestination(this->GetActorPtr(), initLocation, initLocation, world->GetWorldTime(), 62.0f);

	AttackInfos infos;
	infos.push_back(AttackInfo(0, 260, 1000, FVector(110.0f, 100.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Melee, infos);
}

void EnemySlime::OnAutoAttackShot(ActorPtr inVictim)
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

void EnemySlime::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
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
	mMeleeAttack->SetParryinglTime(worldTime, worldTime + 150);

	AttackInfo info = mAutoAttackComponent.GetAttackInfo(0);
	mMeleeAttack->PushTask(worldTime + 200, &EnemyMeleeAttack::CheackCollision);
	mMeleeAttack->PushTask(worldTime + 250, &EnemyMeleeAttack::PushReserveDestroy);

}

void EnemySlime::OnAutoAttackOver()
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

void EnemySlime::OnStunWakeUp()
{
	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() == EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Chase);
		}
	}
}

void EnemySlime::OnReward()
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
	money->SetAmount(static_cast<int32>(Random::GetNormalDistribution(20, 0.6)));

	if (true == Random::GetRandomProbability(1.0, 100.0, 50.0))
	{
		AItemPtr stone = std::static_pointer_cast<AItem>(world->SpawnActor<AItem>(world->GetGameObjectRef(), Random::GetRandomVectorInRange2D(location, 100.0f), FRotator(), FVector()));
		stone->SetItemCode(172);
	}

	PlayerCharacterPtr instigated = std::static_pointer_cast<PlayerCharacter>(this->GetAggroActor().lock());
	if (nullptr == instigated)
	{
		return;
	}

	instigated->GetLevelComponent().AddExperience(static_cast<int32>(Random::GetNormalDistribution(4, 0.8)));
}
