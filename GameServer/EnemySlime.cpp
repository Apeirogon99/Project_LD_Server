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
	SetTick(true, SYSTEM_TICK);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	this->mStatsComponent.SetSyncTime(DEFAULT_TICK);

	this->mCapsuleCollisionComponent.SetOwner(this->GetActorRef());
	this->mCapsuleCollisionComponent.SetBoxCollision(FVector(62.0f, 62.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), DEFAULT_TICK);
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

	//this->GetRotation().ToString();
	//this->GetLocation().ToString();

	this->OnSyncLocation(inDeltaTime);

	this->mStateManager.UpdateState(inDeltaTime);
	this->OnSyncEnemy(inDeltaTime);
}

bool EnemySlime::IsValid()
{
	return GetEnemyID() != 0;
}

void EnemySlime::OnAutoAttackShot()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	wprintf(L"OnAutoAttackShot\n");

	//TODO 추가적인 정보가 필요함
	Protocol::S2C_AttackToPlayer attackPacket;
	attackPacket.set_object_id(this->GetGameObjectID());
	attackPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, attackPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void EnemySlime::OnAutoAttackTargeting()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	wprintf(L"OnAutoAttackTargeting\n");

	FVector		location = this->GetLocation();
	FRotator	rotation = this->GetRotation();

	const float boxExtent = 100.0f;
	const float radius = this->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();
	FVector foward = rotation.GetForwardVector();

	FVector		boxLocation = location + (foward * (radius + boxExtent));
	FVector		extent		= FVector(boxExtent, boxExtent, boxExtent);
	FRotator	orientation = this->GetRotation();

	BoxTrace boxTrace(boxLocation, boxLocation, true, extent, orientation);

	auto playerIter = mPlayerViewers.begin();
	for (playerIter; playerIter != mPlayerViewers.end(); ++playerIter)
	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerIter->get()->GetRemotePlayer());
		PlayerCharacterPtr character = remotePlayer->GetCharacter();

		bool isOverlap = boxTrace.BoxCollisionTrace(character->GetCapsuleCollisionComponent());
		if (isOverlap)
		{
			character->PushTask(worldTime, &Actor::OnHit, this->GetActorPtr(), 10.0f, Location());
		}

	}
}

void EnemySlime::OnAutoAttackOver()
{
	wprintf(L"OnAutoAttackOver\n");
	this->mStateManager.SetState(EStateType::State_Chase);
}
