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
	SetTick(ENEMY_STATE_TICK, true);
	mStateManager.SetEnemy(GetEnemyCharacterRef());
	mStateManager.SetState(EStateType::State_Idle);
}

void EnemySlime::OnDestroy()
{
	std::shared_ptr<EnemySpawner<EnemySlime>> spawner = std::static_pointer_cast<EnemySpawner<EnemySlime>>(GetOwner().lock());
	if (nullptr == spawner)
	{
		return;
	}
	spawner->OnDestroyEnemy(GetGameObjectID());
}

void EnemySlime::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	mStateManager.UpdateState(inDeltaTime);

	Protocol::S2C_TickEnemy tickEnemyPacket;
	tickEnemyPacket.set_object_id(this->GetGameObjectID());
	tickEnemyPacket.mutable_enemy()->CopyFrom(this->ConvertSEnemy());

	SendBufferPtr SendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, tickEnemyPacket);
	this->BrodcastViewers(SendBuffer);

	mMoveSyncTick += inDeltaTime;
	if (mMoveSyncTick > MAX_LOCATION_SYNC_TIME)
	{
		WorldPtr world = GetWorld().lock();
		if (nullptr == world)
		{
			return;
		}

		const int64 serviceTimeStamp = world->GetServiceTimeStamp();
		OnMovement(serviceTimeStamp);
		mMoveSyncTick = 0;
	}
}

bool EnemySlime::IsValid()
{
	return GetEnemyID() != 0;
}

void EnemySlime::OnMovement(const int64 inTimeStamp)
{
	Protocol::S2C_MovementEnemy movementPacket;
	movementPacket.set_object_id(this->GetGameObjectID());
	movementPacket.set_timestamp(inTimeStamp);
	movementPacket.mutable_cur_location()->CopyFrom(this->GetLocation());
	movementPacket.mutable_move_location()->CopyFrom(this->GetMoveLocation());

	SendBufferPtr SendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, movementPacket);
	this->BrodcastViewers(SendBuffer);
}

void EnemySlime::OnHit(const float inDamage, const bool inIsStun)
{
	mStateManager.SetState(EStateType::State_Hit);

	mStats.SetHealth(-inDamage);

	if (mStats.GetHealth() <= DEATH_HEALTH)
	{
		mStateManager.SetState(EStateType::State_Death);
		return;
	}

}
