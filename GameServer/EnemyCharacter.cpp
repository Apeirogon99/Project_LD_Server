#include "pch.h"
#include "EnemyCharacter.h"

EnemyCharacter::EnemyCharacter(const WCHAR* inName) : Character(inName), mEnemyID(0), mSpawnParticle(0), mSpawnParticleDelay(0), mAggressive(false), mStateManager(), mIsReward(true), mMaxChaseRange(0.0f), mMaxSearchRange(0.0f), mIsSpawn(false)
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

	if (mIsReward)
	{
		OnReward();
	}

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

	//if (this->GetPlayerViewers().size() == 0)
	//{
	//	return;
	//}

	//if (false == this->mIsSpawn)
	//{
	//	return;
	//}

	this->OnSyncLocation(inDeltaTime);

	this->mStateManager.UpdateState(inDeltaTime);

	if (this->mStatsComponent.IsChanageStats(inDeltaTime))
	{
		this->DetectChangeEnemy();
	}
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

void EnemyCharacter::OnBeginSpawn()
{
	if (0 == mSpawnParticle || 0 == mSpawnParticleDelay)
	{
		mIsSpawn = true;
		return;
	}

	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(owner->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(this->mSpawnParticle);
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(worldTime + mSpawnParticleDelay, &EnemyCharacter::OnEndSpawn);
}

void EnemyCharacter::OnEndSpawn()
{
	mIsSpawn = true;
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

	if (false == this->GetAggressive())
	{
		this->SetAggroActor(inInstigated);
	}

	const float curArmor	= this->mStatsComponent.GetCurrentStats().GetArmor();
	const float era = curArmor / (curArmor + 100.0f);

	const float curHealth	= this->mStatsComponent.GetCurrentStats().GetHealth() - inDamage * (1.0f - era);
	this->mStatsComponent.UpdateCurrentStat(EStatType::Stat_Health, curHealth);
	if (curHealth <= 0.0f)
	{
		this->mStateManager.SetState(EStateType::State_Death);
		return;
	}

	if (this->mStateManager.GetCurrentStateType() != EStateType::State_Attack && this->mStateManager.GetCurrentStateType() != EStateType::State_Stun)
	{
		this->mStateManager.SetState(EStateType::State_Hit);
	}
	else
	{
		this->DetectChangeEnemy();
	}

	{
		Protocol::S2C_HitEnemy hitPacket;
		hitPacket.set_object_id(this->GetGameObjectID());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, hitPacket);
		this->BrodcastPlayerViewers(sendBuffer);
	}
}

void EnemyCharacter::OnDeath()
{
	const int64 gameObjectID = this->GetGameObjectID();

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	EnemySpawnerPtr spawner = this->mSpawner.lock();
	if (nullptr == spawner)
	{
		return;
	}
	spawner->NotifyDestroyEnemy(gameObjectID);
}

void EnemyCharacter::OnBuffChanage(const EBuffType inBuffType, const EStatType inStatType, const float inValue, bool inIsPush)
{
	if (inIsPush)
	{
		this->mBuffComponent.PushBuff(this->mStatsComponent, inBuffType, inStatType, inValue);
	}
	else
	{
		this->mBuffComponent.ReleaseBuff(this->mStatsComponent, inBuffType, inStatType, inValue);
	}

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

void EnemyCharacter::SetReward(const bool& inIsReward)
{
	mIsReward = inIsReward;
}

void EnemyCharacter::SetAggressive(const bool& inIsAggressive)
{
	mAggressive = inIsAggressive;
}

void EnemyCharacter::SetEnemeyID(const int32 inEnemyID)
{
	mEnemyID = inEnemyID;
}

void EnemyCharacter::SetSpawner(EnemySpawnerRef inSpawner)
{
	mSpawner = inSpawner;
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

void EnemyCharacter::SetMaxSearchRange(const float& inMaxSearchRange)
{
	mMaxSearchRange = inMaxSearchRange;
}

void EnemyCharacter::SetMaxChaseRange(const float& inMaxChaseRange)
{
	mMaxChaseRange = inMaxChaseRange;
}

bool EnemyCharacter::IsDeath() const
{
	return this->mStatsComponent.GetCurrentStats().GetHealth() <= 0.0f;
}
