#include "pch.h"
#include "Character.h"

Character::Character(const RemotePlayerRef& inReomtePlayer) : Pawn(L"Character"), mRemotePlayer(inReomtePlayer), mCharacterID(-1), mIsLoad(false)
{

}

Character::~Character()
{

}

void Character::OnInitialization()
{
	SetTick(100, true);
	mStats.Clear();
}

void Character::OnDestroy()
{
}

void Character::OnTick(const int64 inDeltaTime)
{
	if(false == IsValid())
	{
		return;
	}

	SyncLocation(inDeltaTime);
}

bool Character::IsValid()
{
	return mIsLoad;
}

void Character::AppearActor(PlayerStatePtr inAppearPlayerState)
{
	RemotePlayerPtr targetRemotePlayer = mRemotePlayer.lock();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	if (false == IsLoad())
	{
		return;
	}

	Viewers& viewers = targetRemotePlayer->GetViewers();
	if (viewers.find(inAppearPlayerState) != viewers.end())
	{
		return;
	}
	targetRemotePlayer->GetViewers().insert(inAppearPlayerState);
	inAppearPlayerState->GetMonitors().insert(targetRemotePlayer);

	RemotePlayerPtr appearRemotePlayer = inAppearPlayerState->GetRemotePlayer();
	if (nullptr == appearRemotePlayer)
	{
		return;
	}

	Protocol::S2C_AppearCharacter appearPacket;
	appearPacket.set_remote_id(targetRemotePlayer->GetGameObjectID());
	appearPacket.set_timestamp(this->GetMoveLastTick());
	appearPacket.mutable_cur_location()->CopyFrom(this->GetLocation());
	appearPacket.mutable_move_location()->CopyFrom(this->GetMoveLocation());
	appearPacket.mutable_character_data()->CopyFrom(this->GetCharacterData());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inAppearPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, appearPacket);
	inAppearPlayerState->Send(sendBuffer);
}

void Character::DisAppearActor(PlayerStatePtr inDisappearPlayerState)
{
	RemotePlayerPtr targetRemotePlayer = mRemotePlayer.lock();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	Viewers& viewers = targetRemotePlayer->GetViewers();
	if (viewers.find(inDisappearPlayerState) == viewers.end())
	{
		return;
	}
	targetRemotePlayer->GetViewers().erase(inDisappearPlayerState);

	RemotePlayerPtr disappearRemotePlayer = inDisappearPlayerState->GetRemotePlayer();
	if (nullptr == disappearRemotePlayer)
	{
		return;
	}

	Protocol::S2C_DisAppearCharacter disappearPacket;
	disappearPacket.set_remote_id(targetRemotePlayer->GetGameObjectID());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inDisappearPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disappearPacket);
	inDisappearPlayerState->Send(sendBuffer);
}

//void Character::MoveDestination(Protocol::C2S_MovementCharacter inPacket)
//{
//	RemotePlayerPtr remotePlayer = mRemotePlayer.lock();
//	if (nullptr == remotePlayer)
//	{
//		return;
//	}
//	
//	const Protocol::SVector&	curLocation		= inPacket.cur_location();
//	const Protocol::SVector&	moveLocation	= inPacket.move_location();
//	const int64					remoteID		= remotePlayer->GetGameObjectID();
//	const int64					timestamp		= inPacket.timestamp();
//
//	Protocol::S2C_MovementCharacter newMovementPacket;
//	newMovementPacket.set_remote_id(remoteID);
//
//	Protocol::SVector* oldMovementLocation = newMovementPacket.mutable_cur_location();
//	oldMovementLocation->CopyFrom(curLocation);
//	this->SetLocation(curLocation);
//
//	Protocol::SVector* newMovementLocation = newMovementPacket.mutable_move_location();
//	newMovementLocation->CopyFrom(moveLocation);
//	this->SetMoveLocation(moveLocation);
//
//	newMovementPacket.set_timestamp(timestamp);
//	this->SetMoveLastTick(timestamp);
//
//	PlayerStatePtr playerState = remotePlayer->GetPlayerState().lock();
//	if (nullptr == playerState)
//	{
//		return;
//	}
//
//	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
//	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, newMovementPacket);
//	remotePlayer->BrodcastViewers(sendBuffer);
//}

void Character::SyncLocation(const int64 inDeltaTime)
{
	RemotePlayerPtr remotePlayer = mRemotePlayer.lock();
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerStatePtr playerState = remotePlayer->GetPlayerState().lock();
	if (nullptr == playerState)
	{
		return;
	}

	WorldPtr world = remotePlayer->GetWorldRef().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 serviceTimeStamp = world->GetServiceTimeStamp();

	const float closeToDestination = MAX_LOCATION_DISTANCE;
	const float locationDistance = FVector::Distance2D(PacketUtils::ToFVector(GetLocation()), PacketUtils::ToFVector(GetMoveLocation()));
	if (locationDistance > closeToDestination)
	{
		this->MovingDestination(serviceTimeStamp);
	}

	mMoveSyncTick += inDeltaTime;
	if (mMoveSyncTick > MAX_LOCATION_SYNC_TIME)
	{

		Protocol::S2C_MovementCharacter movementPacket;
		movementPacket.set_remote_id(remotePlayer->GetGameObjectID());
		movementPacket.mutable_cur_location()->CopyFrom(this->GetLocation());
		movementPacket.mutable_move_location()->CopyFrom(this->GetMoveLocation());
		movementPacket.set_timestamp(serviceTimeStamp);

		PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, movementPacket);
		remotePlayer->BrodcastViewers(sendBuffer);

		//wprintf(L"[SEND] (%5.6f:%5.6f:%5.6f)\n", this->GetLocation().x(), this->GetLocation().y(), this->GetLocation().z());

		mMoveSyncTick = 0;

	}
}

void Character::OnMovement()
{
	RemotePlayerPtr remotePlayer = mRemotePlayer.lock();
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerStatePtr playerState = remotePlayer->GetPlayerState().lock();
	if (nullptr == playerState)
	{
		return;
	}

	WorldPtr world = remotePlayer->GetWorldRef().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64 serviceTimeStamp = world->GetServiceTimeStamp();

	Protocol::S2C_MovementCharacter movementPacket;
	movementPacket.set_remote_id(remotePlayer->GetGameObjectID());
	movementPacket.mutable_cur_location()->CopyFrom(this->GetLocation());
	movementPacket.mutable_move_location()->CopyFrom(this->GetMoveLocation());
	movementPacket.set_timestamp(serviceTimeStamp);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, movementPacket);
	remotePlayer->BrodcastViewers(sendBuffer);
}

void Character::SetLoad(bool inIsLoad)
{
	mIsLoad = inIsLoad;
}

void Character::SetCharacterID(const int32& inCharacterID)
{
	mCharacterID = inCharacterID;
}

void Character::SetCharacterData(Protocol::SCharacterData inCharacterData)
{
	mCharacterData.CopyFrom(inCharacterData);
}

void Character::ReplaceEqipment(const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart)
{
	Protocol::SCharacterEqipment* eqipment = mCharacterData.mutable_eqipment();
	if (nullptr == eqipment)
	{
		return;
	}

	const int32 insertInventoryItemCode = inInsertInventoryItem->GetItemCode();
	const int32 insertEqipmentItemCode = inInsertEqipmentItem->GetItemCode();

	static std::function<int32(const int32, const int32, const int32)> compareEqipment = [](const int32 inInstalledItemCode, const int32 inCompareItemCode, const int32 inReplaceItemCode) -> int32 { return inInstalledItemCode == inCompareItemCode ? inReplaceItemCode : inInstalledItemCode; };

	switch (inPart)
	{
	case Protocol::Part_Unspecified:
		break;
	case Protocol::Part_helmet:
		eqipment->set_helmet(compareEqipment(eqipment->helmet(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Shoulders:
		eqipment->set_shoulders(compareEqipment(eqipment->shoulders(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Chest:
		eqipment->set_chest(compareEqipment(eqipment->chest(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Bracers:
		eqipment->set_bracers(compareEqipment(eqipment->bracers(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Hands:
		eqipment->set_hands(compareEqipment(eqipment->hands(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Pants:
		eqipment->set_pants(compareEqipment(eqipment->pants(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Boots:
		eqipment->set_boots(compareEqipment(eqipment->boots(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Weapon_l:
		eqipment->set_weapon_l(compareEqipment(eqipment->weapon_l(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Weapon_r:
		eqipment->set_weapon_r(compareEqipment(eqipment->weapon_r(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	default:
		break;
	}
}

void Character::UpdateStats()
{
	RemotePlayerPtr remotePlayer = mRemotePlayer.lock();
	if (nullptr == remotePlayer)
	{
		return;
	}
	
	DataManagerPtr dataManager = remotePlayer->GetPlayerState().lock()->GetSessionManager()->GetService()->GetDataManager();
	if (nullptr == dataManager)
	{
		return;
	}

	const int32 characterClass	= mCharacterData.character_class();
	const int32 level			= mCharacterData.level();

	CSVRow* baseStats = dataManager->PeekRow(static_cast<uint8>(EGameDataType::BaseStat), characterClass);
	CSVRow* growStats = dataManager->PeekRow(static_cast<uint8>(EGameDataType::GrowStat), characterClass);

	Inventoryptr inventory = remotePlayer->GetInventory();
	const std::vector<AItemPtr>& eqipments = inventory->GetEqipments();

	for (AItemPtr eqipment : eqipments)
	{

		CSVRow* eqipmentStats = dataManager->PeekRow(static_cast<uint8>(EGameDataType::ItemEqipment), eqipment->GetItemCode());

		for (int32 index = 0; index < MAX_STATS_NUM; ++index)
		{
			float baseStat		= stof(baseStats->at(index));
			float growStat		= stof(growStats->at(index));
			float eqipmentStat	= stof(eqipmentStats->at(index));

			mStats.GetStats()[index] = baseStat + (level * growStat) + eqipmentStat;
		}
	}

}

int32 Character::GetEqipmentPartCode(Protocol::ECharacterPart inPart)
{
	int32 partCode = -1;

	Protocol::SCharacterEqipment* eqipment = mCharacterData.mutable_eqipment();
	if (nullptr == eqipment)
	{
		return partCode;
	}

	switch (inPart)
	{
	case Protocol::Part_Unspecified:
		break;
	case Protocol::Part_helmet:
		partCode = eqipment->helmet();
		break;
	case Protocol::Part_Shoulders:
		partCode = eqipment->shoulders();
		break;
	case Protocol::Part_Chest:
		partCode = eqipment->chest();
		break;
	case Protocol::Part_Bracers:
		partCode = eqipment->bracers();
		break;
	case Protocol::Part_Hands:
		partCode = eqipment->hands();
		break;
	case Protocol::Part_Pants:
		partCode = eqipment->pants();
		break;
	case Protocol::Part_Boots:
		partCode = eqipment->boots();
		break;
	case Protocol::Part_Weapon_l:
		partCode = eqipment->weapon_l();
		break;
	case Protocol::Part_Weapon_r:
		partCode = eqipment->weapon_r();
		break;
	default:
		break;
	}

	return partCode;
}
