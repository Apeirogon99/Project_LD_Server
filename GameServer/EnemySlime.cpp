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
	SetTick(true, DEFAULT_TICK);
	mStateManager.SetEnemy(GetEnemyCharacterRef());
	mStateManager.SetState(EStateType::State_Idle);
}

void EnemySlime::OnDestroy()
{
	EnemySpawnerPtr spawner = std::static_pointer_cast<EnemySpawner>(GetOwner().lock());
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

	SyncStatus(inDeltaTime);

}

bool EnemySlime::IsValid()
{
	return GetEnemyID() != 0;
}

void EnemySlime::SyncStatus(const int64 inDeltaTime)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	mSyncStatusTime += inDeltaTime;
	if (mSyncStatusTime <= 1000)
	{
		return;
	}
	mSyncStatusTime = 0;

	//Protocol::S2C_TickEnemy tickPacket;
	//tickPacket.set_object_id(this->GetGameObjectID());
	//tickPacket.set_timestamp(world->GetWorldTime());
	//
	//SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, tickPacket);
	//this->BrodcastPlayerViewers(sendBuffer);
}
