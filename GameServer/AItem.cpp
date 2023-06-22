#include "pch.h"
#include "AItem.h"

AItem::AItem() : Actor(L"[ITEM]")
{
	mItemCode			= 0;
	mInvenPosition.set_x(0);
	mInvenPosition.set_y(0);
	mInvenRotation		= 0;
}

AItem::~AItem()
{
	
}

AItem::AItem(const AItem& inItem) : Actor(L"[ITEM]")
{
	Init(inItem);
}

AItem::AItem(const Protocol::SItem& inItem) : Actor(L"[ITEM]")
{
	Init(inItem);
}

AItem& AItem::operator=(const AItem& inItem)
{
	Init(inItem);
	return *this;
}

AItem& AItem::operator=(const Protocol::SItem& inItem)
{
	Init(inItem);
	return *this;
}

void AItem::Initialization()
{
	mItemCode			= 0;
	mInvenPosition.set_x(0);
	mInvenPosition.set_y(0);
	mInvenRotation		= 0;
}

void AItem::Destroy()
{
}

void AItem::Tick()
{

}

bool AItem::IsValid()
{
	return this->GetGameObjectID() != -1;
}

void AItem::AppearActor(PlayerStatePtr inClosePlayerState)
{
	RemotePlayerPtr targetRemotePlayer = inClosePlayerState->GetRemotePlayer();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	if (0 == GetItemCode())
	{
		return;
	}

	ViewActors& viewActors = targetRemotePlayer->GetViewActors();
	if (viewActors.find(GetGameObjectPtr()) != viewActors.end())
	{
		return;
	}
	targetRemotePlayer->GetViewActors().insert(GetGameObjectPtr());

	Protocol::S2C_AppearItem appearItemPacket;
	appearItemPacket.mutable_item()->CopyFrom(this->ConvertSItem());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inClosePlayerState);
	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, appearItemPacket);
	inClosePlayerState->Send(appearItemSendBuffer);
}

void AItem::DisAppearActor(PlayerStatePtr inClosePlayerState)
{
	RemotePlayerPtr targetRemotePlayer = inClosePlayerState->GetRemotePlayer();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	ViewActors& viewActors = targetRemotePlayer->GetViewActors();
	if (viewActors.find(GetGameObjectPtr()) == viewActors.end())
	{
		return;
	}
	targetRemotePlayer->GetViewActors().erase(GetGameObjectPtr());

	Protocol::S2C_DisAppearGameObject disAppearItemPacket;
	disAppearItemPacket.set_object_id(this->GetGameObjectID());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inClosePlayerState);
	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disAppearItemPacket);
	inClosePlayerState->Send(appearItemSendBuffer);
}

void AItem::Clear()
{
	this->SetGameObjectID(0);

	mItemCode = 0;
	mInvenPosition.set_x(0);
	mInvenPosition.set_y(0);
	mInvenRotation = 0;

	SetLocation(0,0,0);
}

void AItem::Init(const Protocol::SItem& inItem)
{

	this->SetGameObjectID(inItem.object_id());

	mItemCode			= inItem.item_code();
	mInvenPosition		= inItem.inven_position();
	mInvenRotation		= inItem.rotation();

	SetLocation(inItem.world_position());
}

void AItem::Init(const AItem& inItem)
{

	this->SetGameObjectID(inItem.GetGameObjectID());

	mItemCode			= inItem.mItemCode;
	mInvenPosition		= inItem.mInvenPosition;
	mInvenRotation		= inItem.mInvenRotation;

	SetLocation(inItem.GetLocation());
}

void AItem::Init(const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation)
{
	mItemCode			= inItemCode;
	mInvenPosition.set_x(inInvenPositionX);
	mInvenPosition.set_y(inInvenPositionY);
	mInvenRotation		= inRotation;

	SetLocation(inWorldPositionX, inWorldPositionY, inWorldPositionZ);
}

void AItem::Init(const int32 inItemCode, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation)
{
	mItemCode			= inItemCode;
	mInvenPosition.set_x(inInvenPositionX);
	mInvenPosition.set_y(inInvenPositionY);
	mInvenRotation		= inRotation;

}

void AItem::SetItemCode(const int32 inItemCode)
{
	mItemCode = inItemCode;
}

void AItem::SetInventoryPosition(const Protocol::SVector2D& inInventoryPosition)
{
	mInvenPosition.CopyFrom(inInventoryPosition);
}

void AItem::SetInventoryRoation(const int32 inInventoryRotation)
{
	mInvenRotation = inInventoryRotation;
}

const Protocol::SItem AItem::ConvertSItem()
{
	Protocol::SItem tempItem;
	tempItem.set_object_id(this->GetGameObjectID());
	tempItem.set_item_code(this->GetItemCode());
	tempItem.mutable_inven_position()->CopyFrom(this->GetInventoryPosition());
	tempItem.set_rotation(this->GetInventoryRoation());
	tempItem.mutable_world_position()->CopyFrom(this->GetLocation());
	return tempItem;
}
