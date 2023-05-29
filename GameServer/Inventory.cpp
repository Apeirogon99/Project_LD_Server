#include "pch.h"
#include "Inventory.h"

Inventory::Inventory(const RemotePlayerRef& inReomtePlayer, const int32 inInventoryWidth, const int32 inInventoryHeight) : mWidth(inInventoryWidth), mHeight(inInventoryHeight), mRemotePlayer(inReomtePlayer)
{
	mStorage = mWidth * mHeight;
	mInventory = new uint8[mStorage]();
	memset(mInventory, 0, mStorage);
}

Inventory::~Inventory()
{
	mItems.clear();
}

bool Inventory::PushItem(const int64 inObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInventoryPositionX, const int32 inInventoryPositionY, const int32 inRotation)
{
	AItem tempItem = AItem(inObjectID, inItemCode, inWorldPositionX, inWorldPositionY, inWorldPositionZ, inInventoryPositionX, inInventoryPositionY, inRotation);
	
	if (false == AddItem(tempItem))
	{
		return false;
	}

	auto result = mItems.insert(std::make_pair(inObjectID, std::move(tempItem)));
	return result.second;
}

bool Inventory::PushItem(const Protocol::SItem* inItem)
{
	AItem tempItem = AItem(inItem);

	if (false == AddItem(tempItem))
	{
		return false;
	}

	auto result = mItems.insert(std::make_pair(tempItem.mObjectID, std::move(tempItem)));
	return result.second;
}

bool Inventory::PushItem(const AItem& inItem)
{
	if (false == AddItem(inItem))
	{
		return false;
	}

	auto result = mItems.insert(std::make_pair(inItem.mObjectID, inItem));
	return result.second;
}

bool Inventory::LoadItem(Protocol::S2C_LoadInventory& inPacket)
{
	for (auto& item : mItems)
	{
		const AItem& curItem = item.second;

		Protocol::SItem* loadItem = inPacket.add_item();
		loadItem->set_object_id(curItem.mObjectID);
		loadItem->set_item_code(curItem.mItemCode);

		Protocol::SVector* worldPosition = loadItem->mutable_world_position();
		worldPosition->set_x(curItem.mWorldPositonX);
		worldPosition->set_y(curItem.mWorldPositonY);
		worldPosition->set_z(curItem.mWorldPositonZ);

		Protocol::SVector2D* invenPositon = loadItem->mutable_inven_position();
		invenPositon->set_x(curItem.mInventoryPositionX);
		invenPositon->set_y(curItem.mInventoryPositionY);

		loadItem->set_rotation(curItem.mRotation);
	}
	inPacket.set_error(0);
	return true;
}

bool Inventory::UpdateItem(const AItem& inItem)
{
	const int64 objectID = inItem.mObjectID;
	auto findItem = mItems.find(objectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	SubItem(findItem->second);
	findItem->second = inItem;
	AddItem(findItem->second);

	return true;
}

bool Inventory::UpdateItem(const Protocol::SItem& inItem)
{
	const int64 objectID = inItem.object_id();
	auto findItem = mItems.find(objectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	SubItem(findItem->second);
	findItem->second = inItem;
	AddItem(findItem->second);

	return true;
}

bool Inventory::DeleteItem(const AItem& inItem)
{
	const int64 objectID = inItem.mObjectID;
	size_t result = mItems.erase(objectID);
	if (result != 0)
	{
		SubItem(inItem);
		return true;
	}

	return false;
}

bool Inventory::DeleteItem(const Protocol::SItem& inItem)
{
	const int64 objectID = inItem.object_id();
	size_t result = mItems.erase(objectID);
	if (result != 0)
	{
		SubItem(inItem);
		return true;
	}

	return false;
}

bool Inventory::FindItem(const int64 inObjectID, AItem& outItem)
{
	auto findItem = mItems.find(inObjectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	outItem = findItem->second;
	return true;
}

bool Inventory::FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY, AItem& outItem)
{
	for (auto item : mItems)
	{
		const AItem& tempItem = item.second;
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
	PrintInventoryDebug();

	for (int32 index = 0; index < mStorage; ++index)
	{
		if (mInventory[index] > 2)
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

	PlayerStatePtr playerState = remotePlayer->GetPlayerState().lock();
	if (nullptr == playerState)
	{
		return false;
	}

	DataManagerPtr dataManager = playerState->GetSessionManager()->GetService()->GetDataManager();
	dataManager->GetRow(outRow, static_cast<uint8>(EGameDataType::Item), inItemCode);
	return true;
}

CSVRow* Inventory::PeekItemRow(const int32 inItemCode)
{
	RemotePlayerPtr remotePlayer = mRemotePlayer.lock();
	if (nullptr == remotePlayer)
	{
		return nullptr;
	}

	PlayerStatePtr playerState = remotePlayer->GetPlayerState().lock();
	if (nullptr == playerState)
	{
		return nullptr;
	}

	DataManagerPtr dataManager = playerState->GetSessionManager()->GetService()->GetDataManager();
	return dataManager->PeekRow(static_cast<uint8>(EGameDataType::Item), inItemCode);
}

bool Inventory::AddItem(const AItem& item)
{

	CSVRow* row = PeekItemRow(item.mItemCode);
	if (nullptr == row)
	{
		return false;
	}

	int32 itemSizeX = stoi(row->at(static_cast<int32>(EItemCellType::size_x)));
	int32 itemSizeY = stoi(row->at(static_cast<int32>(EItemCellType::size_y)));

	//가로
	if (item.mRotation == 0)
	{
		for (int32 indexY = item.mInventoryPositionY; indexY < item.mInventoryPositionY + itemSizeY; ++indexY)
		{
			for (int32 indexX = item.mInventoryPositionX; indexX < item.mInventoryPositionX + itemSizeX; ++indexX)
			{
				const int32 indexPos = (indexX + (indexY * mWidth));
				mInventory[indexPos] += 1;
			}
		}
	}
	//세로
	else if (item.mRotation == 1)
	{
		for (int32 indexY = item.mInventoryPositionY; indexY < item.mInventoryPositionY + itemSizeX; ++indexY)
		{
			for (int32 indexX = item.mInventoryPositionX; indexX < item.mInventoryPositionX + itemSizeY; ++indexX)
			{
				const int32 indexPos = (indexX + (indexY * mWidth));
				mInventory[indexPos] += 1;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool Inventory::SubItem(const AItem& item)
{
	CSVRow* row = PeekItemRow(item.mItemCode);
	if (nullptr == row)
	{
		return false;
	}

	int32 itemSizeX = stoi(row->at(static_cast<int32>(EItemCellType::size_x)));
	int32 itemSizeY = stoi(row->at(static_cast<int32>(EItemCellType::size_y)));

	//가로
	if (item.mRotation == 0)
	{
		for (int32 indexY = item.mInventoryPositionY; indexY < item.mInventoryPositionY + itemSizeY; ++indexY)
		{
			for (int32 indexX = item.mInventoryPositionX; indexX < item.mInventoryPositionX + itemSizeX; ++indexX)
			{
				const int32 indexPos = (indexX + (indexY * mWidth));
				mInventory[indexPos] -= 1;
			}
		}
	}
	//세로
	else if (item.mRotation == 1)
	{
		for (int32 indexY = item.mInventoryPositionY; indexY < item.mInventoryPositionY + itemSizeX; ++indexY)
		{
			for (int32 indexX = item.mInventoryPositionX; indexX < item.mInventoryPositionX + itemSizeY; ++indexX)
			{
				const int32 indexPos = (indexX + (indexY * mWidth));
				mInventory[indexPos] -= 1;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

void Inventory::PrintInventoryDebug()
{
	for (int32 i = 0; i < mStorage; ++i)
	{
		if (i % mWidth == 0)
		{
			printf("\n");
		}

		printf("[%hhu]", mInventory[i]);
	}
	printf("\n");
}
