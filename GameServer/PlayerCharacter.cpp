#include "pch.h"
#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter() : Character(L"PlayerCharacter"), mCharacterID(0), mIsLoad(false)
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

	AttackInfos infos;
	infos.push_back(AttackInfo(500,		170,	1100, FVector(100.0f, 100.0f, 100.0f)));
	infos.push_back(AttackInfo(710,		180,	1100, FVector(100.0f, 100.0f, 100.0f)));
	infos.push_back(AttackInfo(0,		310,	1000, FVector(100.0f, 100.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Combo_Melee, infos);
}

void PlayerCharacter::OnDestroy()
{
}

void PlayerCharacter::OnTick(const int64 inDeltaTime)
{
	if(false == IsValid())
	{
		return;
	}

	if (false == this->mMovementComponent.Update(this->GetActorPtr(), 10.0f))
	{
		this->SetVelocity(0.0f, 0.0f, 0.0f);
	}
	this->SyncLocation(inDeltaTime);

	//this->mAutoAttackComponent.Update(this->GetActorPtr());

	//this->GetRotation().ToString();
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

void PlayerCharacter::MovementCharacter(Protocol::C2S_MovementCharacter pkt)
{

	Location	currentLocation		= PacketUtils::ToFVector(pkt.cur_location());
	Location	movementDestination = PacketUtils::ToFVector(pkt.move_location());
	int64		movementLastTime	= pkt.timestamp();

	float distance = FVector::Distance2D(currentLocation, this->GetLocation());
	if (distance >= 42.0f)
	{
		return;
	}

	this->mMovementComponent.SetNewDestination(this->GetActorPtr(), currentLocation, movementDestination, movementLastTime, 42.0f);

	OnMovement();
}

void PlayerCharacter::OnMovement()
{
	const float movementSpeed = this->mStatComponent.GetCurrentStats().GetMovementSpeed();
	this->SetVelocity(movementSpeed, movementSpeed, movementSpeed);

	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
		const int64	remoteID = remotePlayer->GetGameObjectID();
		const int64	movementLastTime = this->mMovementComponent.GetLastMovementTime();

		const Protocol::SVector& currentLocation = PacketUtils::ToSVector(this->GetLocation());
		const Protocol::SVector& movementDestination = PacketUtils::ToSVector(this->mMovementComponent.GetDestinationLocation());

		Protocol::S2C_MovementCharacter movementPacket;
		movementPacket.set_remote_id(remoteID);
		movementPacket.mutable_cur_location()->CopyFrom(currentLocation);
		movementPacket.mutable_move_location()->CopyFrom(movementDestination);
		movementPacket.set_timestamp(movementLastTime);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, movementPacket);
		remotePlayer->BrodcastPlayerViewers(sendBuffer);
	}
}

void PlayerCharacter::AutoAttack(Protocol::C2S_PlayerAutoAttack pkt)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	const int64 worldTime		= world->GetWorldTime();
	const int64 objectID		= pkt.object_id();
	const int64 attackTimeStmap = pkt.timestamp();

	ActorPtr victimActor = nullptr;
	if (false == world->FindActor(objectID, victimActor))
	{
		return;
	}

	const Stats& currentStat = mStatComponent.GetCurrentStats();
	const float range = currentStat.GetRange();

	if (false == this->mAutoAttackComponent.IsAutoAttackRange(this->GetActorPtr(), victimActor, range))
	{
		FVector curLocation = this->GetLocation();
		FVector victimLocation = victimActor->GetLocation();
		this->mMovementComponent.SetNewDestination(this->GetActorPtr(), curLocation, victimLocation, worldTime, range);
		OnMovement();
	}
	else
	{

		if (false == this->mAutoAttackComponent.IsComboAutoAttacking(this->GetActorPtr()))
		{
			return;
		}

		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
		const int64	remoteID = remotePlayer->GetGameObjectID();

		const float damage = currentStat.GetAttackDamage();
		const int64 overTime = StatUtils::CoolTime(currentStat.GetAttackSpeed(), 0.0f, 0.0f, 0.0f);

		const int32 autoAttackCount = this->mAutoAttackComponent.GetAutoAttackCount();
		this->mAutoAttackComponent.DoComboMeleeAutoAttack(this->GetActorPtr(), victimActor, damage);

		Protocol::S2C_PlayerAutoAttack autoAttackPacket;
		autoAttackPacket.set_remote_id(remoteID);
		autoAttackPacket.set_combo(autoAttackCount);
		autoAttackPacket.set_timestamp(worldTime);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
		remotePlayer->BrodcastPlayerViewers(sendBuffer);
	}

}

void PlayerCharacter::OnHit(ActorPtr inInstigated, const float inDamage)
{
	printf("Player Hit\n");
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
	
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	const int64	remoteID = remotePlayer->GetGameObjectID();

	const Stats& currentStat	= mStatComponent.GetCurrentStats();
	const float damage			= currentStat.GetAttackDamage();
	const int64 overTime		= StatUtils::CoolTime(currentStat.GetAttackSpeed(), 0.0f, 0.0f, 0.0f);

	const int32 autoAttackCount = this->mAutoAttackComponent.GetAutoAttackCount();
	this->mAutoAttackComponent.DoComboMeleeAutoAttack(this->GetActorPtr(), inVictim, damage);

	Protocol::S2C_PlayerAutoAttack autoAttackPacket;
	autoAttackPacket.set_remote_id(remoteID);
	autoAttackPacket.set_combo(autoAttackCount);
	autoAttackPacket.set_timestamp(worldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
	remotePlayer->BrodcastPlayerViewers(sendBuffer);

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

	FVector		location	= this->GetLocation();
	FRotator	rotation	= this->GetRotation();
	FVector		foward		= rotation.GetForwardVector();
	const float radius		= this->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();

	FVector		boxLocation = location + (foward * (radius + inRange.GetX()));
	BoxTrace	boxTrace(boxLocation, boxLocation, true, inRange, rotation);

	uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(boxLocation, radius, findActorType, findActors);
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
}

void PlayerCharacter::SetCharacterID(const int32& inCharacterID)
{
	mCharacterID = inCharacterID;
}

void PlayerCharacter::SetCharacterData(Protocol::SCharacterData inCharacterData)
{
	mCharacterData.CopyFrom(inCharacterData);
}

//void PlayerCharacter::ReplaceEqipment(const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart)
//{
//	Protocol::SCharacterEqipment* eqipment = mCharacterData.mutable_eqipment();
//	if (nullptr == eqipment)
//	{
//		return;
//	}
//
//	const int32 insertInventoryItemCode = inInsertInventoryItem->GetItemCode();
//	const int32 insertEqipmentItemCode = inInsertEqipmentItem->GetItemCode();
//
//	static std::function<int32(const int32, const int32, const int32)> compareEqipment = [](const int32 inInstalledItemCode, const int32 inCompareItemCode, const int32 inReplaceItemCode) -> int32 { return inInstalledItemCode == inCompareItemCode ? inReplaceItemCode : inInstalledItemCode; };
//
//	switch (inPart)
//	{
//	case Protocol::Part_Unspecified:
//		break;
//	case Protocol::Part_helmet:
//		eqipment->set_helmet(compareEqipment(eqipment->helmet(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	case Protocol::Part_Shoulders:
//		eqipment->set_shoulders(compareEqipment(eqipment->shoulders(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	case Protocol::Part_Chest:
//		eqipment->set_chest(compareEqipment(eqipment->chest(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	case Protocol::Part_Bracers:
//		eqipment->set_bracers(compareEqipment(eqipment->bracers(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	case Protocol::Part_Hands:
//		eqipment->set_hands(compareEqipment(eqipment->hands(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	case Protocol::Part_Pants:
//		eqipment->set_pants(compareEqipment(eqipment->pants(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	case Protocol::Part_Boots:
//		eqipment->set_boots(compareEqipment(eqipment->boots(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	case Protocol::Part_Weapon_l:
//		eqipment->set_weapon_l(compareEqipment(eqipment->weapon_l(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	case Protocol::Part_Weapon_r:
//		eqipment->set_weapon_r(compareEqipment(eqipment->weapon_r(), insertInventoryItemCode, insertEqipmentItemCode));
//		break;
//	default:
//		break;
//	}
//}
//
//int32 PlayerCharacter::GetEqipmentPartCode(Protocol::ECharacterPart inPart)
//{
//	int32 partCode = -1;
//
//	Protocol::SCharacterEqipment* eqipment = mCharacterData.mutable_eqipment();
//	if (nullptr == eqipment)
//	{
//		return partCode;
//	}
//
//	switch (inPart)
//	{
//	case Protocol::Part_Unspecified:
//		break;
//	case Protocol::Part_helmet:
//		partCode = eqipment->helmet();
//		break;
//	case Protocol::Part_Shoulders:
//		partCode = eqipment->shoulders();
//		break;
//	case Protocol::Part_Chest:
//		partCode = eqipment->chest();
//		break;
//	case Protocol::Part_Bracers:
//		partCode = eqipment->bracers();
//		break;
//	case Protocol::Part_Hands:
//		partCode = eqipment->hands();
//		break;
//	case Protocol::Part_Pants:
//		partCode = eqipment->pants();
//		break;
//	case Protocol::Part_Boots:
//		partCode = eqipment->boots();
//		break;
//	case Protocol::Part_Weapon_l:
//		partCode = eqipment->weapon_l();
//		break;
//	case Protocol::Part_Weapon_r:
//		partCode = eqipment->weapon_r();
//		break;
//	default:
//		break;
//	}
//
//	return partCode;
//}
