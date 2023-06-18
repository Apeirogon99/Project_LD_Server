#include "pch.h"
#include "Character.h"

Character::Character(const RemotePlayerRef& inReomtePlayer) : Pawn(L"Character"), mRemotePlayer(inReomtePlayer), mCharacterID(-1), mLastMovementTimeStamp(0)
{

}

Character::~Character()
{

}

void Character::Initialization()
{
	mLastMovementTimeStamp = 0;
}

void Character::Destroy()
{
}

void Character::Tick()
{
}

bool Character::IsValid()
{
	return true;
}

void Character::AppearActor(PlayerStatePtr inAppearPlayerState)
{
	RemotePlayerPtr targetRemotePlayer = mRemotePlayer.lock();
	if (nullptr == targetRemotePlayer)
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
	appearPacket.set_timestamp(this->mLastMovementTimeStamp);
	appearPacket.mutable_old_location()->CopyFrom(this->mOldLocation);
	appearPacket.mutable_new_location()->CopyFrom(this->GetLocation());
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

void Character::MoveDestination(Protocol::C2S_MovementCharacter inPakcet)
{
	RemotePlayerPtr remotePlayer = mRemotePlayer.lock();
	if (nullptr == remotePlayer)
	{
		return;
	}
	
	const Protocol::SVector&	oldLocation = inPakcet.old_location();
	const Protocol::SVector&	newLocation	= inPakcet.new_location();
	const int64					remoteID	= remotePlayer->GetGameObjectID();
	const int64					timestamp	= inPakcet.timestamp();

	this->mLastMovementTimeStamp = timestamp;
	this->mOldLocation.CopyFrom(oldLocation);
	this->SetLocation(newLocation);

	Protocol::S2C_MovementCharacter newMovementPacket;
	Protocol::SVector* oldMovementLocation = newMovementPacket.mutable_old_location();
	oldMovementLocation->CopyFrom(oldLocation);

	Protocol::SVector* newMovementLocation = newMovementPacket.mutable_new_location();
	newMovementLocation->CopyFrom(newLocation);

	newMovementPacket.set_remote_id(remoteID);
	newMovementPacket.set_timestamp(timestamp);

	PlayerStatePtr playerState = remotePlayer->GetPlayerState().lock();
	if (nullptr == playerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, newMovementPacket);
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
