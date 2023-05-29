#include "pch.h"
#include "AItem.h"

AItem::AItem() : mObjectID(-1), mItemCode(-1), mWorldPositonX(-1), mWorldPositonY(-1), mWorldPositonZ(-1), mInventoryPositionX(-1), mInventoryPositionY(-1), mRotation(-1)
{
}

AItem::AItem(const int64 inObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInventoryPositionX, const int32 inInventoryPositionY, const int32 inRotation) : mObjectID(inObjectID), mItemCode(inItemCode), mWorldPositonX(inWorldPositionX), mWorldPositonY(inWorldPositionY), mWorldPositonZ(inWorldPositionZ), mInventoryPositionX(inInventoryPositionX), mInventoryPositionY(inInventoryPositionY), mRotation(inRotation)
{
}

AItem::AItem(const Protocol::SItem* inItem)
{
	mObjectID = inItem->object_id();
	mItemCode = inItem->item_code();

	const Protocol::SVector& worldPosition = inItem->world_position();
	mWorldPositonX = worldPosition.x();
	mWorldPositonY = worldPosition.y();
	mWorldPositonZ = worldPosition.z();

	const Protocol::SVector2D& inventoryPosition = inItem->inven_position();
	mInventoryPositionX = inventoryPosition.x();
	mInventoryPositionY = inventoryPosition.y();

	mRotation = inItem->rotation();
}

AItem::~AItem()
{
}

AItem::AItem(const AItem& inOtherItem)
{
	this->mObjectID = inOtherItem.mObjectID;
	this->mItemCode = inOtherItem.mItemCode;
	this->mWorldPositonX = inOtherItem.mWorldPositonX;
	this->mWorldPositonY = inOtherItem.mWorldPositonY;
	this->mWorldPositonZ = inOtherItem.mWorldPositonZ;
	this->mInventoryPositionX = inOtherItem.mInventoryPositionX;
	this->mInventoryPositionY = inOtherItem.mInventoryPositionY;
	this->mRotation = inOtherItem.mRotation;
}

AItem::AItem(const Protocol::SItem& inOtherItem)
{
	mObjectID = inOtherItem.object_id();
	mItemCode = inOtherItem.item_code();

	const Protocol::SVector& worldPosition = inOtherItem.world_position();
	mWorldPositonX = worldPosition.x();
	mWorldPositonY = worldPosition.y();
	mWorldPositonZ = worldPosition.z();

	const Protocol::SVector2D& inventoryPosition = inOtherItem.inven_position();
	mInventoryPositionX = inventoryPosition.x();
	mInventoryPositionY = inventoryPosition.y();

	mRotation = inOtherItem.rotation();
}

AItem::AItem(AItem&& inOtherItem) noexcept
{
	this->mObjectID = inOtherItem.mObjectID;
	this->mItemCode = inOtherItem.mItemCode;
	this->mWorldPositonX = inOtherItem.mWorldPositonX;
	this->mWorldPositonY = inOtherItem.mWorldPositonY;
	this->mWorldPositonZ = inOtherItem.mWorldPositonZ;
	this->mInventoryPositionX = inOtherItem.mInventoryPositionX;
	this->mInventoryPositionY = inOtherItem.mInventoryPositionY;
	this->mRotation = inOtherItem.mRotation;
}

AItem& AItem::operator=(const AItem& inOtherItem)
{
	this->mObjectID = inOtherItem.mObjectID;
	this->mItemCode = inOtherItem.mItemCode;
	this->mWorldPositonX = inOtherItem.mWorldPositonX;
	this->mWorldPositonY = inOtherItem.mWorldPositonY;
	this->mWorldPositonZ = inOtherItem.mWorldPositonZ;
	this->mInventoryPositionX = inOtherItem.mInventoryPositionX;
	this->mInventoryPositionY = inOtherItem.mInventoryPositionY;
	this->mRotation = inOtherItem.mRotation;
	return *this;
}

AItem& AItem::operator=(const Protocol::SItem& inOtherItem)
{
	this->mObjectID = inOtherItem.object_id();
	this->mItemCode = inOtherItem.item_code();

	const Protocol::SVector& worldPosition = inOtherItem.world_position();
	this->mWorldPositonX = worldPosition.x();
	this->mWorldPositonY = worldPosition.y();
	this->mWorldPositonZ = worldPosition.z();
	
	const Protocol::SVector2D& inventoryPosition = inOtherItem.inven_position();
	this->mInventoryPositionX = inventoryPosition.x();
	this->mInventoryPositionY = inventoryPosition.y();

	this->mRotation = inOtherItem.rotation();
	return *this;
}

AItem& AItem::operator=(AItem&& inOtherItem) noexcept
{
	this->mObjectID = inOtherItem.mObjectID;
	this->mItemCode = inOtherItem.mItemCode;
	this->mWorldPositonX = inOtherItem.mWorldPositonX;
	this->mWorldPositonY = inOtherItem.mWorldPositonY;
	this->mWorldPositonZ = inOtherItem.mWorldPositonZ;
	this->mInventoryPositionX = inOtherItem.mInventoryPositionX;
	this->mInventoryPositionY = inOtherItem.mInventoryPositionY;
	this->mRotation = inOtherItem.mRotation;
	return *this;
}

//void AItem::Initialization()
//{
//	mObjectID			= -1;
//	mItemCode			= -1;
//	mWorldPositonX		= -1;
//	mWorldPositonY		= -1;
//	mWorldPositonZ		= -1;
//	mInventoryPositionX = -1;
//	mInventoryPositionY = -1;
//	mRotation			= -1;
//}
//
//void AItem::Destroy()
//{
//}
//
//void AItem::Tick()
//{
//}
//
//bool AItem::IsValid()
//{
//	return false;
//}
