#include "pch.h"
#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter() : Character(L"PlayerCharacter"), mCharacterID(0), mIsLoad(false), mPlayerMode(EPlayerMode::Unspecified_MODE)
{

}

PlayerCharacter::~PlayerCharacter()
{
	printf("~PlayerCharacter\n");
}

void PlayerCharacter::OnInitialization()
{
	GameRemotePlayerPtr owner = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	SetTick(true, SYSTEM_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mStatComponent.SetSyncTime(GAME_TICK);
	this->mSkillComponent.Init(owner, SECOND_TICK);
	this->mBuffComponent.Init(owner);

	AttackInfos infos;
	infos.push_back(AttackInfo(500,		170,	1100, FVector(100.0f, 150.0f, 100.0f)));
	infos.push_back(AttackInfo(710,		180,	1100, FVector(100.0f, 150.0f, 100.0f)));
	infos.push_back(AttackInfo(0,		310,	1000, FVector(180.0f, 100.0f, 150.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Combo_Melee, infos);

	this->SetPlayerMode(EPlayerMode::Move_MODE);

	this->SetActorType(static_cast<uint8>(EActorType::Player));

	//TEMP
	this->mSkillComponent.PushSkill(1);
	this->mSkillComponent.PushSkill(2);
	this->mSkillComponent.PushSkill(3);
	this->mSkillComponent.PushSkill(4);
	this->mSkillComponent.PushSkill(5);

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

	const float debugDuration = 0.02f;
	PacketUtils::DebugDrawSphere(std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock())->GetViewers(), this->GetLocation(), 42.0f, debugDuration);

	if (true == this->mMovementComponent.Update(this->GetActorPtr(), 42.0f))
	{
		this->NextPlayerMode();
	}
	this->SyncLocation(inDeltaTime);

	if (this->mStatComponent.IsChanageStats(inDeltaTime))
	{
		this->DetectChangePlayer();
	}
	this->mStatComponent.UpdateStats(inDeltaTime);

	this->mSkillComponent.UpdateSkillCoolTime(inDeltaTime);

	//printf("[%lld] remote [%d] player : ", this->GetGameObjectID(), this->GetCharacterID()); this->GetLocation().ToString();
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


	const Stats& stats = mStatComponent.GetCurrentStats();
	for (int32 index = 0; index < MAX_STATS_NUM; ++index)
	{
		Protocol::SStat* addStat = appearPacket.add_stats();
		addStat->set_stat_type(static_cast<Protocol::EStatType>(index + 1));
		addStat->set_stat_value(stats.GetStat(index));
	}


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

void PlayerCharacter::NextPlayerMode()
{
	EPlayerMode mode = this->mPlayerMode;

	if (mode == EPlayerMode::Move_MODE)
	{
		return;
	}
	this->SetPlayerMode(EPlayerMode::Move_MODE);

	ActorPtr target = mTargetActor.lock();
	if (nullptr != target)
	{
		if (mode == EPlayerMode::PickUp_MODE)
		{
			std::static_pointer_cast<AItem>(target)->PickUp(std::static_pointer_cast<PlayerCharacter>(this->shared_from_this()));
		}
		else if (mode == EPlayerMode::Attack_MODE)
		{
			this->AutoAttack(target->GetGameObjectID());
		}
		else if (mode == EPlayerMode::Skill_MODE)
		{

		}
	}
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

	OnMovement();
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

void PlayerCharacter::Teleport(FVector inDestinationLocation)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	this->GetMovementComponent().SetNewDestination(this->GetActorPtr(), inDestinationLocation, inDestinationLocation, worldTime, 0.0f);

	remotePlayer->OnLoadComplete();
	
}

void PlayerCharacter::MovementCharacter(Protocol::C2S_MovementCharacter pkt)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	Location	serverLocation = this->GetLocation();
	int64		servertMovementLastTime	= this->mMovementComponent.GetLastMovementTime();

	Location	clientLocation = PacketUtils::ToFVector(pkt.cur_location());
	int64		clientMovementLastTime	= pkt.timestamp();

	Location	movementDestination		= PacketUtils::ToFVector(pkt.move_location());
	float		radius = this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();

	bool isRestrict = this->mMovementComponent.GetRestrictMovement();
	if (true == isRestrict)
	{
		return;
	}

	int64		serverDuration			= clientMovementLastTime - servertMovementLastTime;
	Location	currentServerLocation	= this->mMovementComponent.GetNextLocation(this->GetActorPtr(), serverLocation, this->mMovementComponent.GetServerDestinationLocation(), serverDuration, 0.0f);

	{
		this->mMovementComponent.SetNewDestination(this->GetActorPtr(), currentServerLocation, movementDestination, clientMovementLastTime, 0.0f);
	}
	//else
	//{
	//	printf("CLIENT\n");
	//	this->mMovementComponent.SetNewDestination(this->GetActorPtr(), currentClientLocation, movementDestination, clientMovementLastTime, 42.0f);
	//}
	

	mTargetActor.reset();
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

	EnemyCharacterPtr enemy = std::static_pointer_cast<EnemyCharacter>(victimActor);
	if (nullptr == enemy)
	{
		return;
	}

	const Stats& currentStat = mStatComponent.GetCurrentStats();
	const float range = currentStat.GetRange();

	if (false == this->mAutoAttackComponent.IsAutoAttackRange(this->GetActorPtr(), victimActor, range))
	{

		const float half = this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ();
		const float victimHalf = enemy->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ();

		FVector curLocation		= this->GetLocation();
		FVector victimLocation	= enemy->GetMovementComponent().GetNextLocation(enemy);
		victimLocation.SetZ(curLocation.GetZ());

		this->mMovementComponent.SetNewDestination(this->GetActorPtr(), curLocation, victimLocation, worldTime, 0.0f);
		this->SetPlayerMode(EPlayerMode::Attack_MODE);
		this->SetTargetActor(enemy);
		OnMovement();
		return;
	}
	this->mTargetActor.reset();

	if (true == this->mAutoAttackComponent.CanAutoAttack(this->GetActorPtr()))
	{
		this->DoAutoAttack(victimActor);
	}

}

void PlayerCharacter::DoAutoAttack(ActorPtr inVictimActor)
{
	this->mMovementComponent.SetRestrictMovement(true);

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int64		remoteID	= remotePlayer->GetGameObjectID();

	const Stats&	currentStat = mStatComponent.GetCurrentStats();
	const float		damage		= currentStat.GetAttackDamage();

	this->GetLocation().ToString();

	const int32 autoAttackCount = this->mAutoAttackComponent.GetAutoAttackCount();
	this->mAutoAttackComponent.DoComboMeleeAutoAttack(this->GetActorPtr(), inVictimActor, damage);

	FVector destinationLocation = this->GetLocation() + this->GetRotation().GetForwardVector() * 1.0f;
	this->mMovementComponent.SetNewDestination(this->GetActorPtr(), this->GetLocation(), destinationLocation, worldTime, 0.0f);
	this->OnMovement();

	{
		Protocol::SRotator	rotation = PacketUtils::ToSRotator(FRotator(0.0f, this->GetRotation().GetYaw(), 0.0f));
		Protocol::SVector	location = PacketUtils::ToSVector(this->GetLocation());

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

	ActiveSkillPtr activeSKill = this->GetSkillComponent().GetActiveSkill().lock();
	if (nullptr != activeSKill)
	{
		activeSKill->OnHit(inInstigated, inDamage);
		return;
	}

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

	if (false == this->mAutoAttackComponent.IsComboAutoAttack())
	{
		this->mAutoAttackComponent.DoComboMeleeAutoAttackOVer(this->GetActorPtr());
	}
	else
	{
		this->DoAutoAttack(inVictim);
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

	FVector			location	= this->GetLocation();
	FRotator		rotation	= FRotator(0.0f, this->GetRotation().GetYaw(), 0.0f);
	FVector			foward		= rotation.GetForwardVector();
	const float		collision	= this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();
	const float		radius		= std::sqrtf(std::powf(inRange.GetX(), 2) + std::powf(inRange.GetY(), 2));	//외접원 반지름

	Location		boxStartLocation	= location; // +(foward * collision); 앞에 두고 싶으면 추가해야함
	Location		boxEndLocation		= boxStartLocation + (foward * (inRange.GetX() * 2));
	Location		boxCenterLocation	= (boxStartLocation + boxEndLocation) / 2.0f;
	BoxTrace		boxTrace(this->GetActorRef(), boxStartLocation, boxEndLocation, true, inRange, rotation);

	//DEBUG
	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawBox(remotePlayer->GetRemoteClient().lock(), boxStartLocation, boxEndLocation, inRange, debugDuration);
	PacketUtils::DebugDrawSphere(remotePlayer->GetRemoteClient().lock(), boxCenterLocation, radius, debugDuration);

	uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(boxTrace, findActorType, findActors);
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

		enemy->PushTask(worldTime, &Actor::OnHit, this->GetActorPtr(), inDamage);
		
	}
}

void PlayerCharacter::OnAutoAttackOver()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int64& remoteID = remotePlayer->GetGameObjectID();

	this->SetPlayerMode(EPlayerMode::Move_MODE);
	this->mAutoAttackComponent.OnOverAutoComboAttack();
	this->mMovementComponent.SetRestrictMovement(false);

	Protocol::S2C_PlayerEndAutoAttack endAutoAttackPacket;
	endAutoAttackPacket.set_remote_id(remoteID);
	endAutoAttackPacket.set_timestamp(worldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, endAutoAttackPacket);
	remotePlayer->BrodcastPlayerViewers(sendBuffer);
}

int32 PlayerCharacter::UseSkill(const int32 inSkillID, float inUseMana)
{
	if (false == mSkillComponent.CanUseSkill(inSkillID))
	{
		return static_cast<int32>(EDCommonErrorType::COOL_TIME_IS_NOT_ENOUGH);
	}

	float mana = mStatComponent.GetCurrentStats().GetMana();
	if (mana - inUseMana < 0.0f)
	{
		return static_cast<int32>(EDCommonErrorType::MANA_IS_NOT_ENOUGH);
	}
	mStatComponent.UpdateCurrentStat(EStatType::Stat_Mana, mana - inUseMana);

	return static_cast<int32>(EDCommonErrorType::SUCCESS);
}

void PlayerCharacter::OnBuffChanage(const EBuffType inBuffType, const EStatType inStatType, const float inValue, bool inIsPush)
{
	if (inIsPush)
	{
		this->mBuffComponent.PushBuff(this->mStatComponent, inBuffType, inStatType, inValue);
	}
	else
	{
		this->mBuffComponent.ReleaseBuff(this->mStatComponent, inBuffType, inStatType, inValue);
	}
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
