#include "pch.h"
#include "AItem.h"

AItem::AItem(const int64 inGameObjectID) : Actor(L"[AITEM]", inGameObjectID)
{
	Initialization();
}

AItem::AItem(const int64 inGameObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation) : Actor(typeid(this).name(), inGameObjectID), mItemCode(inItemCode), mInventoryPositionX(inInvenPositionX), mInventoryPositionY(inInvenPositionY), mRotation(inRotation)
{
	SetLocation(inWorldPositionX, inWorldPositionY, inWorldPositionZ);
}

AItem::~AItem()
{
	Destroy();
}

AItem::AItem(const AItem& inItem) : Actor(inItem.GetActorName(), inItem.GetGameObjectID())
{
	mItemCode				= inItem.mItemCode;
	mLocation				= inItem.mLocation;
	mInventoryPositionX		= inItem.mInventoryPositionX;
	mInventoryPositionY		= inItem.mInventoryPositionY;
	mRotation				= inItem.mRotation;
}

AItem::AItem(const Protocol::SItem& inItem) : Actor(L"[AITEM]", inItem.object_id())
{
	Init(inItem);
}

AItem& AItem::operator=(const AItem& inItem)
{
	mActorName				= inItem.mActorName;
	mGameObjectID			= inItem.mGameObjectID;
	mItemCode				= inItem.mItemCode;
	mLocation				= inItem.mLocation;
	mInventoryPositionX		= inItem.mInventoryPositionX;
	mInventoryPositionY		= inItem.mInventoryPositionY;
	mRotation				= inItem.mRotation;
	return *this;
}

AItem& AItem::operator=(const Protocol::SItem& inItem)
{
	//SetActorName(L"[AITEM]"));
	Init(inItem);
	return *this;
}

void AItem::Initialization()
{
	mItemCode			= -1;
	mInventoryPositionX	= -1;
	mInventoryPositionY	= -1;
	mRotation			= -1;
}

void AItem::Destroy()
{
}

void AItem::Tick()
{

}

bool AItem::IsValid()
{
	return true;
}

void AItem::Init(const Protocol::SItem& inItem)
{
	mItemCode			= inItem.item_code();
	mLocation			= inItem.world_position();
	mInventoryPositionX = inItem.inven_position().x();
	mInventoryPositionY = inItem.inven_position().y();
	mRotation			= inItem.rotation();
}
