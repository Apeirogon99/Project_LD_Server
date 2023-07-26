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
	SetTick(true, SYSTEM_TICK);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	this->mStatsComponent.SetSyncTime(DEFAULT_TICK);

	this->mCapsuleCollisionComponent.SetOwner(this->GetActorRef());
	this->mCapsuleCollisionComponent.SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), DEFAULT_TICK);

	AttackInfos infos;
	infos.push_back(AttackInfo(0, 650, 1600, FVector(100.0f, 100.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Melee, infos);
}

void EnemyNomalSkeleton::OnAutoAttackShot(ActorPtr inVictim)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	Protocol::SRotator rotation = PacketUtils::ToSRotator(this->GetRotation());

	Protocol::S2C_EnemyAutoAttack autoAttackPacket;
	autoAttackPacket.set_object_id(this->GetGameObjectID());
	autoAttackPacket.mutable_rotation()->CopyFrom(rotation);
	autoAttackPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void EnemyNomalSkeleton::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

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

void EnemyNomalSkeleton::OnAutoAttackOver()
{
	this->mAutoAttackComponent.OnOverAutoAttack();
	this->mStateManager.SetState(EStateType::State_Chase);
}
