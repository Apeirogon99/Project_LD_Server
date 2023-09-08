#include "pch.h"
#include "LichLifeVessel.h"

LichLifeVessel::LichLifeVessel() : EnemyCharacter(L"LifeVessel")
{
}

LichLifeVessel::~LichLifeVessel()
{
}

void LichLifeVessel::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	this->SetEnemeyID(static_cast<int32>(EnemyID::Enemy_Lich_Life_Vessle));

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (datas)
	{
		this->mStatsComponent.SetSyncTime(GAME_TICK);
		this->mStatsComponent.InitMaxStats(datas->GetEnemyStat(static_cast<int32>(EnemyID::Enemy_Lich_Life_Vessle)));
	}

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(30.0f, 30.0f, 48.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

}

void LichLifeVessel::OnDestroy()
{
	OnReward();
}

void LichLifeVessel::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	this->OnSyncLocation(inDeltaTime);

	this->mStateManager.SetState(EStateType::State_Idle);
	this->mStateManager.UpdateState(inDeltaTime);

	if (this->mStatsComponent.IsChanageStats(inDeltaTime))
	{
		this->DetectChangeEnemy();
	}
}

void LichLifeVessel::OnReward()
{
}
