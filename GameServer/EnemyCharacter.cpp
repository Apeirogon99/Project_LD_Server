#include "pch.h"
#include "EnemyCharacter.h"

EnemyCharacter::EnemyCharacter(const WCHAR* inName) : Character(inName), mEnemyID(0), mSpawnObjectID(0), mStateManager()
{
}

EnemyCharacter::~EnemyCharacter()
{
}

void EnemyCharacter::OnDestroy()
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

	{
		Protocol::S2C_DeathEnemy deathPacket;
		deathPacket.set_object_id(this->GetGameObjectID());
		deathPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, deathPacket);
		this->BrodcastPlayerViewers(sendBuffer);
	}

}

void EnemyCharacter::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	this->OnSyncLocation(inDeltaTime);

	this->mStateManager.UpdateState(inDeltaTime);

}

bool EnemyCharacter::IsValid()
{
	bool isLoad = GetEnemyID() != 0;
	bool isAlive = 0.0f <= this->mStatsComponent.GetCurrentStats().GetHealth();
	return isLoad && isAlive;
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

	FVector				curLocation		= this->mMovementComponent.GetCurrentLocation(this->GetActorPtr());
	FVector				moveLocation	= this->mMovementComponent.GetDestinationLocation();
	const int64			worldTime		= this->GetWorld().lock()->GetWorldTime();

	Protocol::S2C_AppearEnemy appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.set_enemy_id(this->GetEnemyID());
	appearPacket.set_state(static_cast<Protocol::EEnemyState>(this->mStateManager.GetCurrentStateType()));
	appearPacket.mutable_cur_location()->CopyFrom(PacketUtils::ToSVector(curLocation));
	appearPacket.mutable_move_location()->CopyFrom(PacketUtils::ToSVector(moveLocation));
	appearPacket.set_timestamp(worldTime);

	std::map<EStatType, float> differentStats;
	if (true == this->mStatsComponent.ExtractDifferentMaxStats(differentStats))
	{
		auto stat = differentStats.begin();
		for (stat; stat != differentStats.end(); ++stat)
		{
			Protocol::SStat* addStat = appearPacket.add_stats();
			addStat->set_stat_type(static_cast<Protocol::EStatType>(stat->first));
			addStat->set_stat_value(stat->second);
		}
	}

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

void EnemyCharacter::OnSyncLocation(const int64 inDeltaTime)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	bool isSync = this->mMovementComponent.SyncUpdate(this->GetActorPtr(), inDeltaTime);
	if (false == isSync)
	{
		return;
	}

	OnMovementEnemy();

}

void EnemyCharacter::DetectChangeEnemy()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	//this->mStateManager.StateChangeDebugPrint();

	Protocol::S2C_DetectChangeEnemy detectChanagePacket;
	detectChanagePacket.set_object_id(this->GetGameObjectID());
	detectChanagePacket.set_timestamp(world->GetWorldTime());
	detectChanagePacket.set_state(static_cast<Protocol::EEnemyState>(this->mStateManager.GetCurrentStateType()));

	std::map<EStatType, float> chanageStats;
	if (true == this->mStatsComponent.ExtractChanageMaxStats(chanageStats))
	{
		auto stat = chanageStats.begin();
		for (stat; stat != chanageStats.end(); ++stat)
		{
			Protocol::SStat* addStat = detectChanagePacket.add_stats();
			addStat->set_stat_type(static_cast<Protocol::EStatType>(stat->first));
			addStat->set_stat_value(stat->second);
		}

	}

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, detectChanagePacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void EnemyCharacter::OnHit(ActorPtr inInstigated, const float inDamage)
{

	if (IsDeath())
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	PlayerCharacterRef instigated = std::static_pointer_cast<PlayerCharacter>(inInstigated);
	this->mAggroActor = instigated;

	const float curHealth = this->mStatsComponent.GetCurrentStats().GetHealth() - inDamage;
	this->mStatsComponent.UpdateCurrentStat(EStatType::Stat_Health, curHealth);
	
	if (curHealth <= 0.0f)
	{
		this->mStateManager.SetState(EStateType::State_Death);
	}
	else
	{
		this->mStateManager.SetState(EStateType::State_Hit);

		Protocol::S2C_HitEnemy hitPacket;
		hitPacket.set_object_id(this->GetGameObjectID());
		hitPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, hitPacket);
		this->BrodcastPlayerViewers(sendBuffer);
	}
}

void EnemyCharacter::OnDeath()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();
	const int64 deathTime = worldTime + 2000;
	const int64 gameObjectID = this->GetGameObjectID();

	world->PushTask(deathTime, &World::DestroyActor, gameObjectID);
}

void EnemyCharacter::OnMovementEnemy()
{
	Protocol::S2C_MovementEnemy movementPacket;
	movementPacket.set_object_id(this->GetGameObjectID());
	movementPacket.mutable_cur_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	movementPacket.mutable_move_location()->CopyFrom(PacketUtils::ToSVector(this->mMovementComponent.GetDestinationLocation()));
	movementPacket.set_timestamp(this->mMovementComponent.GetLastMovementTime());
	
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, movementPacket);
	this->BrodcastPlayerViewers(sendBuffer);
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

void EnemyCharacter::SetAggroActor(ActorRef inAggroActor)
{
	mAggroActor = inAggroActor;
}

bool EnemyCharacter::IsDeath() const
{
	return this->mStatsComponent.GetCurrentStats().GetHealth() <= 0.0f;
}
