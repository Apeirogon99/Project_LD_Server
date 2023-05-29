#include "pch.h"
#include "Inventory.h"

Item::Item() : mObjectID(-1), mItemCode(-1), mWorldPositonX(-1), mWorldPositonY(-1), mWorldPositonZ(-1), mInventoryPositionX(-1), mInventoryPositionY(-1), mRotation(-1)
{
}

Item::Item(const int64 inObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInventoryPositionX, const int32 inInventoryPositionY, const int32 inRotation) : mObjectID(inObjectID), mItemCode(inItemCode), mWorldPositonX(inWorldPositionX), mWorldPositonY(inWorldPositionY), mWorldPositonZ(inWorldPositionZ), mInventoryPositionX(inInventoryPositionX), mInventoryPositionY(inInventoryPositionY), mRotation(inRotation)
{
}

Item::Item(const Protocol::SItem* inItem)
{
	mObjectID			= inItem->object_id();
	mItemCode			= inItem->item_code();

	const Protocol::SVector& worldPosition = inItem->world_position();
	mWorldPositonX		= worldPosition.x();
	mWorldPositonY		= worldPosition.y();
	mWorldPositonZ		= worldPosition.z();

	const Protocol::SVector2D& inventoryPosition = inItem->inven_position();
	mInventoryPositionX = inventoryPosition.x();
	mInventoryPositionY = inventoryPosition.y();

	mRotation			= inItem->rotation();
}

Item::~Item()
{
}

Item::Item(const Item& inOtherItem)
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

Item::Item(Item&& inOtherItem) noexcept
{
	this->mObjectID				= inOtherItem.mObjectID;
	this->mItemCode				= inOtherItem.mItemCode;
	this->mWorldPositonX		= inOtherItem.mWorldPositonX;
	this->mWorldPositonY		= inOtherItem.mWorldPositonY;
	this->mWorldPositonZ		= inOtherItem.mWorldPositonZ;
	this->mInventoryPositionX	= inOtherItem.mInventoryPositionX;
	this->mInventoryPositionY	= inOtherItem.mInventoryPositionY;
	this->mRotation				= inOtherItem.mRotation;
}

Item& Item::operator=(const Item& inOtherItem)
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

Item& Item::operator=(Item&& inOtherItem) noexcept
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

bool Item::IsValid()
{
	return (mItemCode != -1);
}

Inventory::Inventory(const RemotePlayerRef& inReomtePlayer, const int32 inInventoryWidth, const int32 inInventoryHeight) : mWidth(inInventoryWidth + 1), mHeight(inInventoryHeight + 1), mRemotePlayer(inReomtePlayer)
{
	mStorage = mWidth * mHeight;
	mInventory = new uint8[mStorage]();
}

Inventory::~Inventory()
{
	mItems.clear();
}

bool Inventory::PushItem(const int64 inObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInventoryPositionX, const int32 inInventoryPositionY, const int32 inRotation)
{
	Item tempItem = Item(inObjectID, inItemCode, inWorldPositionX, inWorldPositionY, inWorldPositionZ, inInventoryPositionX, inInventoryPositionY, inRotation);
	
	if (false == AddItem(tempItem))
	{
		return false;
	}

	auto result = mItems.insert(std::make_pair(inObjectID, std::move(tempItem)));
	return result.second;
}

bool Inventory::PushItem(const Protocol::SItem* inItem)
{
	Item tempItem = Item(inItem);

	if (false == AddItem(tempItem))
	{
		return false;
	}

	auto result = mItems.insert(std::make_pair(tempItem.mObjectID, std::move(tempItem)));
	return result.second;
}

bool Inventory::PushItem(const Item& inItem)
{
	if (false == AddItem(inItem))
	{
		return false;
	}

	auto result = mItems.insert(std::make_pair(inItem.mObjectID, inItem));
	return result.second;
}

bool Inventory::LoadItem(std::unordered_map<int64, Item>& inItems)
{
	inItems = mItems;
	return true;
}

bool Inventory::UpdateItem(const Item& inItem)
{
	const int64 objectID = inItem.mObjectID;
	auto findItem = mItems.find(objectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	findItem->second = inItem;
	return true;
}

bool Inventory::DeleteItem(const Item& inItem)
{
	const int64 objectID = inItem.mObjectID;
	size_t result = mItems.erase(objectID);
	return (result != 0) ? true : false;
}

bool Inventory::FindItem(const int64 inObjectID, Item& outItem)
{
	auto findItem = mItems.find(inObjectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	outItem = findItem->second;
	return true;
}

bool Inventory::FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY, Item& outItem)
{
	for (auto item : mItems)
	{
		const Item& tempItem = item.second;
		if (tempItem.mItemCode == inItemCode && tempItem.mInventoryPositionX == inInventoryPositionX && tempItem.mInventoryPositionY == inInventoryPositionY)
		{
			outItem = tempItem;
			return true;
		}
	}

	return false;
}

bool Inventory::CheckInventory()
{

	for (int32 index = 0; index < mStorage; ++index)
	{
		if (index > 2)
		{
			return false;
		}
	}

	return true;
}

bool Inventory::GetItemRow(const int32 inItemCode, CSVRow& outRow)
{
	RemotePlayerPtr remotePlayer = mRemotePlayer.lock();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	PlayerStatePtr playerState = remotePlayer->mPlayerState.lock();
	if (nullptr == playerState)
	{
		return false;
	}

	DataManagerPtr dataManager = playerState->GetSessionManager()->GetService()->GetDataManager();
	dataManager->GetRow(outRow, static_cast<uint8>(EGameDataType::Item), inItemCode);
	return true;
}

bool Inventory::AddItem(const Item& item)
{

	CSVRow row;
	if (false == GetItemRow(item.mItemCode, row))
	{
		return false;
	}

	int32 itemSizeX = stoi(row[static_cast<size_t>(EItemCellType::size_x)]);
	int32 itemSizeY = stoi(row[static_cast<size_t>(EItemCellType::size_y)]);

	//가로
	if (item.mRotation == 0)
	{
		for (int32 indexX = item.mInventoryPositionX; indexX < item.mInventoryPositionX + itemSizeX; ++indexX)
		{
			for (int32 indexY = item.mInventoryPositionY; indexY < item.mInventoryPositionY + itemSizeY; ++indexY)
			{
				const int32 indexPos = (indexX * indexY);
				mInventory[indexPos] += 1;
			}
		}
	}
	//세로
	else if (item.mRotation == 1)
	{
		for (int32 indexX = item.mInventoryPositionX; indexX < item.mInventoryPositionX + itemSizeY; ++indexX)
		{
			for (int32 indexY = item.mInventoryPositionY; indexY < item.mInventoryPositionY + itemSizeX; ++indexY)
			{
				const int32 indexPos = (indexX * indexY);
				mInventory[indexPos] += 1;
			}
		}
	}
	else
	{
		return false;
	}

	if (false == CheckInventory())
	{
		return false;
	}

	return true;
}
