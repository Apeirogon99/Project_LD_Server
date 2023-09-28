#include "pch.h"
#include "EnemyChest.h"

EnemyChest::EnemyChest() : EnemyCharacter(L"EnemyChest")
{
}

EnemyChest::~EnemyChest()
{
}

void EnemyChest::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	this->SetEnemeyID(static_cast<int32>(EnemyID::Enemy_Chest));

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(30.0f, 30.0f, 48.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());
}

void EnemyChest::OnReward()
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
}
