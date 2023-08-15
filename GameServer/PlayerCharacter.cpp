#include "pch.h"
#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter() : Character(L"PlayerCharacter"), mCharacterID(0), mIsLoad(false), mPlayerMode(EPlayerMode::Unspecified_MODE)
{

}

PlayerCharacter::~PlayerCharacter()
{

}

void PlayerCharacter::OnInitialization()
{
	SetTick(true, SYSTEM_TICK);
	this->mCapsuleCollisionComponent.SetOwner(this->GetActorRef());
	this->mCapsuleCollisionComponent.SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mStatComponent.SetSyncTime(GAME_TICK);

	AttackInfos infos;
	infos.push_back(AttackInfo(500,		170,	1100, FVector(130.0f, 300.0f, 300.0f)));
	infos.push_back(AttackInfo(710,		180,	1100, FVector(130.0f, 300.0f, 300.0f)));
	infos.push_back(AttackInfo(0,		310,	1000, FVector(180.0f, 150.0f, 150.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Combo_Melee, infos);

	this->mMovementComponent.SetRestrictMovement(true);

	this->SetPlayerMode(EPlayerMode::Move_MODE);

	this->SetActorType(static_cast<uint8>(EActorType::Player));

	//TEMP
	this->mSkillComponent.PushSkill(1);
	this->mSkillComponent.PushSkill(2);
	this->mSkillComponent.PushSkill(3);
	this->mSkillComponent.PushSkill(4);
}

void PlayerCharacter::OnDestroy()
{
	mIsLoad = false;
}

void PlayerCharacter::OnTick(const int64 inDeltaTime)
{
	if(false == IsValid())
	{
		return;
	}

	float vel = this->mStatComponent.GetCurrentStats().GetMovementSpeed();
	this->SetVelocity(vel, vel, vel);

	if (false == this->mMovementComponent.Update(this->GetActorPtr(), 10.0f))
	{
		ActorPtr target = mTargetActor.lock();
		if (nullptr != target)
		{
			if (this->mPlayerMode == EPlayerMode::PickUp_MODE)
			{
				AItemPtr item = std::static_pointer_cast<AItem>(target);
				std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock())->GetInventory()->InsertItemToInventory(item->GetGameObjectID(), item->GetItemCode(), item->GetLocation(), item->GetInventoryPosition(), item->GetInventoryRoation(), item->GetAmount());
			}
			else if (this->mPlayerMode == EPlayerMode::Attack_MODE)
			{
				this->AutoAttack(target->GetGameObjectID());
			}
			else if (this->mPlayerMode == EPlayerMode::Skill_MODE)
			{
			}

			this->mTargetActor.reset();
		}

		this->SetPlayerMode(EPlayerMode::Move_MODE);
	}
	this->SyncLocation(inDeltaTime);

	if (this->mStatComponent.IsChanageStats(inDeltaTime))
	{
		this->DetectChangePlayer();
	}

	this->mSkillComponent.UpdateSkillCoolTime(inDeltaTime);
}

bool PlayerCharacter::IsValid()
{
	bool isAlive = 0.0f < this->mStatComponent.GetCurrentStats().GetHealth();
	return mIsLoad && isAlive;
}

void PlayerCharacter::SetLoadCharacter(bool inIsLoad)
{
	mIsLoad = inIsLoad;
}

void PlayerCharacter::OnAppearActor(ActorPtr inAppearActor)
{
	GameRemotePlayerPtr targetRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	if (false == IsValid())
	{
		return;
	}

	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inAppearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	GameRemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (true == targetRemotePlayer->FindPlayerViewer(anotherPlayerState))
	{
		return;
	}
	targetRemotePlayer->InsertPlayerViewer(anotherPlayerState);
	anotherPlayerState->InsertPlayerMonitor(targetRemotePlayer);

	Protocol::S2C_AppearCharacter appearPacket;
	appearPacket.set_remote_id(targetRemotePlayer->GetGameObjectID());
	appearPacket.set_timestamp(this->mMovementComponent.GetLastMovementTime());
	appearPacket.mutable_cur_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearPacket.mutable_move_location()->CopyFrom(PacketUtils::ToSVector(this->mMovementComponent.GetDestinationLocation()));
	appearPacket.mutable_character_data()->CopyFrom(this->GetCharacterData());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearPacket);
	anotherPlayerState->Send(sendBuffer);
}

void PlayerCharacter::OnDisAppearActor(ActorPtr inDisappearActor)
{
	RemotePlayerPtr targetRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inDisappearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	GameRemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == targetRemotePlayer->FindPlayerViewer(anotherPlayerState))
	{
		return;
	}
	targetRemotePlayer->ReleasePlayerViewer(anotherPlayerState);
	anotherPlayerState->ReleasePlayerMonitor(targetRemotePlayer);

	Protocol::S2C_DisAppearCharacter disappearPacket;
	disappearPacket.set_remote_id(targetRemotePlayer->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearPacket);
	anotherPlayerState->Send(sendBuffer);
}

void PlayerCharacter::SyncLocation(const int64 inDeltaTime)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	bool isSync = this->mMovementComponent.SyncUpdate(this->GetActorPtr(), inDeltaTime);
	if (false == isSync)
	{
		return;
	}

	const int64			remoteID			= remotePlayer->GetGameObjectID();
	Protocol::SVector	currentLocation		= PacketUtils::ToSVector(this->GetLocation());
	Protocol::SVector	destinationLocation	= PacketUtils::ToSVector(this->mMovementComponent.GetDestinationLocation());
	const int64			worldTime			= this->mMovementComponent.GetLastMovementTime();


	Protocol::S2C_MovementCharacter movementPacket;
	movementPacket.set_remote_id(remoteID);
	movementPacket.mutable_cur_location()->CopyFrom(currentLocation);
	movementPacket.mutable_move_location()->CopyFrom(destinationLocation);
	movementPacket.set_timestamp(worldTime);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, movementPacket);
	remotePlayer->BrodcastPlayerViewers(sendBuffer);

	//wprintf(L"[SEND] (%5.6f:%5.6f:%5.6f)\n", this->GetLocation().GetX(), this->GetLocation().GetY(), this->GetLocation().GetZ());
}

void PlayerCharacter::DetectChangePlayer()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	Protocol::S2C_DetectChangePlayer detectChanagePacket;
	detectChanagePacket.set_remote_id(remotePlayer->GetGameObjectID());
	detectChanagePacket.set_timestamp(world->GetWorldTime());

	std::map<EStatType, float> chanageStats;
	if (true == this->mStatComponent.ExtractChanageMaxStats(chanageStats))
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
	remotePlayer->BrodcastPlayerViewers(sendBuffer);
}

void PlayerCharacter::MovementCharacter(Protocol::C2S_MovementCharacter pkt)
{
	Location	serverLocation		= this->mMovementComponent.GetCurrentLocation(this->GetActorPtr());
	Location	clientLocation		= PacketUtils::ToFVector(pkt.cur_location());
	Location	movementDestination = PacketUtils::ToFVector(pkt.move_location());
	int64		movementLastTime	= pkt.timestamp();
	float		radius = this->mCapsuleCollisionComponent.GetBoxCollision().GetBoxExtent().GetX();

	if (false == this->mMovementComponent.GetRestrictMovement())
	{
		return;
	}

	float distance = FVector::Distance2D(serverLocation, clientLocation);
	if (distance <= 50.0f)
	{
		this->mMovementComponent.SetNewDestination(this->GetActorPtr(), clientLocation, movementDestination, movementLastTime, radius);
	}
	else
	{
		this->mMovementComponent.SetNewDestination(this->GetActorPtr(), serverLocation, movementDestination, movementLastTime, radius);
	}

	OnMovement();
}

void PlayerCharacter::OnMovement()
{

	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
		const int64	remoteID = remotePlayer->GetGameObjectID();
		const int64	movementLastTime = this->mMovementComponent.GetLastMovementTime();

		const Protocol::SVector& currentLocation = PacketUtils::ToSVector(this->GetLocation());
		const Protocol::SVector& movementDestination = PacketUtils::ToSVector(this->mMovementComponent.GetServerDestinationLocation());

		Protocol::S2C_MovementCharacter movementPacket;
		movementPacket.set_remote_id(remoteID);
		movementPacket.mutable_cur_location()->CopyFrom(currentLocation);
		movementPacket.mutable_move_location()->CopyFrom(movementDestination);
		movementPacket.set_timestamp(movementLastTime);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, movementPacket);
		remotePlayer->BrodcastPlayerViewers(sendBuffer);
	}
}

void PlayerCharacter::AutoAttack(const int64 inAttackingObjectID)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	ActorPtr victimActor = nullptr;
	if (false == world->FindActor(inAttackingObjectID, victimActor))
	{
		return;
	}

	const Stats& currentStat = mStatComponent.GetCurrentStats();
	const float range = currentStat.GetRange();

	if (false == this->mAutoAttackComponent.IsAutoAttackRange(this->GetActorPtr(), victimActor, range))
	{
		FVector curLocation		= this->GetLocation();
		FVector victimLocation	= victimActor->GetLocation();
		this->mMovementComponent.SetNewDestination(this->GetActorPtr(), curLocation, victimLocation, worldTime, range);
		this->SetPlayerMode(EPlayerMode::Attack_MODE);
		this->SetTargetActor(victimActor->GetActorRef());
		OnMovement();
		return;
	}

	if (false == this->mAutoAttackComponent.IsComboAutoAttacking(this->GetActorPtr()))
	{
		return;
	}

	this->mMovementComponent.SetRestrictMovement(false);

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	const int64	remoteID = remotePlayer->GetGameObjectID();

	const float damage = currentStat.GetAttackDamage();
	const int64 overTime = StatUtils::CoolTime(currentStat.GetAttackSpeed(), 0.0f, 0.0f, 0.0f);

	const int32 autoAttackCount = this->mAutoAttackComponent.GetAutoAttackCount();
	this->mAutoAttackComponent.DoComboMeleeAutoAttack(this->GetActorPtr(), victimActor, damage);

	Protocol::SRotator	rotation = PacketUtils::ToSRotator(FRotator(0.0f, this->GetRotation().GetYaw(), 0.0f));
	Protocol::SVector	location = PacketUtils::ToSVector(this->GetLocation());

	{
		Protocol::S2C_PlayerAutoAttack autoAttackPacket;
		autoAttackPacket.set_remote_id(remoteID);
		autoAttackPacket.set_combo(autoAttackCount);
		autoAttackPacket.mutable_location()->CopyFrom(location);
		autoAttackPacket.mutable_rotation()->CopyFrom(rotation);
		autoAttackPacket.set_timestamp(worldTime);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
		remotePlayer->BrodcastPlayerViewers(sendBuffer);
	}
	

}

void PlayerCharacter::OnHit(ActorPtr inInstigated, const float inDamage)
{
	float currentHP = this->mStatComponent.GetCurrentStats().GetHealth();
	this->mStatComponent.UpdateCurrentStat(EStatType::Stat_Health, currentHP - inDamage);
}

void PlayerCharacter::OnAutoAttackShot(ActorPtr inVictim)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	EnemyCharacterPtr enemy = std::static_pointer_cast<EnemyCharacter>(inVictim);
	if (nullptr == enemy)
	{
		return;
	}

	if (false == this->mAutoAttackComponent.CheckComboShotAutoAttack(this->GetActorPtr()))
	{
		return;
	}

	this->mMovementComponent.SetRestrictMovement(false);
	
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	const int64	remoteID = remotePlayer->GetGameObjectID();

	const Stats& currentStat	= mStatComponent.GetCurrentStats();
	const float damage			= StatUtils::RandomDamage(currentStat.GetAttackDamage());
	const int64 overTime		= StatUtils::CoolTime(currentStat.GetAttackSpeed(), 0.0f, 0.0f, 0.0f);

	const int32 autoAttackCount = this->mAutoAttackComponent.GetAutoAttackCount();
	this->mAutoAttackComponent.DoComboMeleeAutoAttack(this->GetActorPtr(), inVictim, damage);

	Protocol::SRotator rotation = PacketUtils::ToSRotator(FRotator(0.0f, this->GetRotation().GetYaw(), 0.0f));
	Protocol::SVector	location = PacketUtils::ToSVector(this->GetLocation());

	{
		Protocol::S2C_PlayerAutoAttack autoAttackPacket;
		autoAttackPacket.set_remote_id(remoteID);
		autoAttackPacket.set_combo(autoAttackCount);
		autoAttackPacket.mutable_location()->CopyFrom(location);
		autoAttackPacket.mutable_rotation()->CopyFrom(rotation);
		autoAttackPacket.set_timestamp(worldTime);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
		remotePlayer->BrodcastPlayerViewers(sendBuffer);
	}

}

void PlayerCharacter::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	FVector		location	= this->mMovementComponent.GetCurrentLocation(this->GetActorPtr());
	FRotator	rotation	= FRotator(0.0f, this->GetRotation().GetYaw(), 0.0f);
	FVector		foward		= rotation.GetForwardVector();
	const float collision	= this->mCapsuleCollisionComponent.GetBoxCollision().GetBoxExtent().GetX();
	const float radius		= (0.5f * std::sqrtf(std::powf(inRange.GetX(), 2) + std::powf(inRange.GetY(), 2)));	//외접원 반지름

	const FVector minusCollision = foward * collision;
	const FVector addRange = foward * inRange.GetX();
	Location	boxCenterLocation = location - minusCollision + addRange;
	BoxTrace	boxTrace(boxCenterLocation, boxCenterLocation, true, inRange, rotation);

	uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(boxCenterLocation, radius, findActorType, findActors);
	if (!result)
	{
		return;
	}

	for (ActorPtr actor : findActors)
	{
		EnemyCharacterPtr enemy = std::static_pointer_cast<EnemyCharacter>(actor);
		if (nullptr == enemy)
		{
			continue;
		}

		bool isOverlap = boxTrace.BoxCollisionTrace(enemy->GetCapsuleCollisionComponent());
		if (isOverlap)
		{
			enemy->PushTask(worldTime, &Actor::OnHit, this->GetActorPtr(), inDamage);
		}

	}
}

void PlayerCharacter::OnAutoAttackOver()
{
	this->mAutoAttackComponent.OnOverAutoComboAttack();
	this->mMovementComponent.SetRestrictMovement(true);
}

void PlayerCharacter::SetCharacterID(const int32& inCharacterID)
{
	mCharacterID = inCharacterID;
}

void PlayerCharacter::SetCharacterData(Protocol::SCharacterData inCharacterData)
{
	mCharacterData.CopyFrom(inCharacterData);
}

void PlayerCharacter::SetPlayerMode(const EPlayerMode& inPlayerMode)
{
	mPlayerMode = inPlayerMode;
}

void PlayerCharacter::SetTargetActor(ActorRef inTargetActor)
{
	mTargetActor = inTargetActor;
}
