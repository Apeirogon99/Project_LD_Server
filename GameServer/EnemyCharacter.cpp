#include "pch.h"
#include "EnemyCharacter.h"

EnemyCharacter::EnemyCharacter(const WCHAR* inName) : Character(inName), mEnemyID(0), mSpawnObjectID(0), mStateManager()
{
}

EnemyCharacter::~EnemyCharacter()
{
}

bool EnemyCharacter::IsValid()
{
	return GetEnemyID() != 0;
}

void EnemyCharacter::OnAppearActor(ActorPtr inAppearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inAppearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == IsValid())
	{
		return;
	}

	if (mPlayerViewers.find(anotherPlayerState) != mPlayerViewers.end())
	{
		return;
	}
	this->InsertPlayerViewer(anotherPlayerState);
	anotherPlayerState->InsertActorMonitor(this->GetActorPtr());

	Protocol::S2C_AppearEnemy appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.set_enemy_id(this->GetEnemyID());
	appearPacket.mutable_enemy()->CopyFrom(this->ConvertSEnemy());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearPacket);
	anotherPlayerState->Send(sendBuffer);
}

void EnemyCharacter::OnDisAppearActor(ActorPtr inDisappearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inDisappearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (mPlayerViewers.find(anotherPlayerState) == mPlayerViewers.end())
	{
		return;
	}
	this->ReleasePlayerViewer(anotherPlayerState);
	anotherPlayerState->ReleaseActorMonitor(this->GetActorPtr());

	Protocol::S2C_DisAppearGameObject disAppearItemPacket;
	disAppearItemPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disAppearItemPacket);
	anotherPlayerState->Send(sendBuffer);
}

void EnemyCharacter::OnHit(ActorPtr inInstigated, const float inDamage, const Location inHitLocation)
{
	PlayerCharacterPtr instigated = std::static_pointer_cast<PlayerCharacter>(inInstigated);
	this->mAggroPlayer = instigated;

	const float curHealth = this->mStatsComponent.GetCurrentStats().GetHealth() - inDamage;
	this->mStatsComponent.UpdateCurrentStat(EStat::health, curHealth);
	
	if (0.0f <= curHealth)
	{
		this->mStateManager.SetState(EStateType::State_Hit);
	}
	else
	{
		this->mStateManager.SetState(EStateType::State_Death);
	}

}

void EnemyCharacter::OnDeath()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	Protocol::S2C_DeathEnemy deathPacket;
	deathPacket.set_object_id(this->GetGameObjectID());
	deathPacket.set_timestamp(world->GetWorldTime());
	
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, deathPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void EnemyCharacter::OnAutoAttackShot()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	Protocol::S2C_AttackToPlayer attackPacket;
	attackPacket.set_object_id(this->GetGameObjectID());
	attackPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, attackPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void EnemyCharacter::OnAutoAttackTargeting()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	//TODO: Push Hit players

	Protocol::S2C_TargetingToPlayer targetingPacket;
	//deathPacket.set_object_id(this->GetGameObjectID());
	//deathPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, targetingPacket);
	this->BrodcastPlayerViewers(appearItemSendBuffer);
}

void EnemyCharacter::OnAutoAttackOver()
{
	//TODO : Attack Again
}

void EnemyCharacter::OnMovementEnemy()
{
	Protocol::S2C_MovementEnemy movementPacket;
	movementPacket.set_object_id(this->GetGameObjectID());
	movementPacket.mutable_cur_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	movementPacket.mutable_move_location()->CopyFrom(PacketUtils::ToSVector(this->mMovementComponent.GetDestinationLocation()));
	movementPacket.set_timestamp(this->mMovementComponent.GetLastMovementTime());
	
	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, movementPacket);
	this->BrodcastPlayerViewers(appearItemSendBuffer);
}

void EnemyCharacter::SetEnemeyID(const int32 inEnemyID)
{
	mEnemyID = inEnemyID;
}

void EnemyCharacter::SetSpawnObjectID(const int64 inSpawnObjectID)
{
	mSpawnObjectID = inSpawnObjectID;
}

void EnemyCharacter::SetRecoveryLocation(const Location& inRecoveryLocation)
{
	mRecoveryLocation = inRecoveryLocation;
}

void EnemyCharacter::SetEnemyStats(const Stats& inEnemyStats)
{
	mStatsComponent.InitMaxStats(inEnemyStats);
}

void EnemyCharacter::SetAggroPlayer(PlayerCharacterPtr inCharacter)
{
	mAggroPlayer = inCharacter;
}

const Protocol::SEnemy EnemyCharacter::ConvertSEnemy()
{
	Protocol::SEnemy tempEnemy;
	tempEnemy.set_state(static_cast<Protocol::EEnemyState>(this->mStateManager.GetCurrentStateType()));
	tempEnemy.set_hp(this->mStatsComponent.GetCurrentStats().GetHealth());
	tempEnemy.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	tempEnemy.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	return tempEnemy;
}
