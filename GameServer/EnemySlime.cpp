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

	AttackInfos infos;
	infos.push_back(AttackInfo(0, 460, 870, FVector(100.0f, 100.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Melee, infos);
}

void EnemySlime::OnDestroy()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	EnemySpawnerPtr spawner = std::static_pointer_cast<EnemySpawner>(GetOwner().lock());
	if (nullptr == spawner)
	{
		return;
	}
	spawner->OnDestroyEnemy(GetGameObjectID());

	Protocol::S2C_DeathEnemy deathEnemyPacket;
	deathEnemyPacket.set_object_id(this->GetGameObjectID());
	deathEnemyPacket.set_timestamp(worldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, deathEnemyPacket);
	this->BrodcastPlayerViewers(sendBuffer);
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

	//OnHit(nullptr, 10000.0f);
}

bool EnemySlime::IsValid()
{
	bool isLoad = GetEnemyID() != 0;
	bool isAlive = 0.0f <= this->mStatsComponent.GetCurrentStats().GetHealth();
	return isLoad && isAlive;
}

void EnemySlime::OnAutoAttackShot(ActorPtr inVictim)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	wprintf(L"OnAutoAttackShot\n");

	Protocol::SRotator rotation = PacketUtils::ToSRotator(this->GetRotation());

	Protocol::S2C_EnemyAutoAttack autoAttackPacket;
	autoAttackPacket.set_object_id(this->GetGameObjectID());
	autoAttackPacket.mutable_rotation()->CopyFrom(rotation);
	autoAttackPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void EnemySlime::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
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
	FVector		foward = rotation.GetForwardVector();
	const float radius = this->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();

	FVector		boxLocation = location + (foward * (radius + inRange.GetX()));
	FRotator	orientation = this->GetRotation();

	BoxTrace boxTrace(boxLocation, boxLocation, true, inRange, orientation);

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

void EnemySlime::OnAutoAttackOver()
{
	wprintf(L"OnAutoAttackOver\n");
	this->mAutoAttackComponent.OnOverAutoAttack();
	this->mStateManager.SetState(EStateType::State_Chase);
}
