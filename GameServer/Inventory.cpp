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

bool Inventory::LoadItem(Protocol::S2C_LoadInventory& inPacket)
{
	for (auto& item : mItems)
	{
		const AItem& curItem = item.second;

		Protocol::SItem* loadItem = inPacket.add_item();
		loadItem->set_object_id(curItem.GetGameObjectID());
		loadItem->set_item_code(curItem.mItemCode);

		Protocol::SVector* worldPosition = loadItem->mutable_world_position();
		*worldPosition = curItem.GetLocation();

		Protocol::SVector2D* invenPositon = loadItem->mutable_inven_position();
		invenPositon->set_x(curItem.mInventoryPositionX);
		invenPositon->set_y(curItem.mInventoryPositionY);

		loadItem->set_rotation(curItem.mRotation);
	}
	inPacket.set_error(0);
	return true;
}

bool Inventory::InsertItem(const AItem& inItem)
{
	if (false == AddItem(inItem))
	{
		return false;
	}

	const int64 gameObjectID = inItem.GetGameObjectID();
	std::pair<int64, AItem> newItem = std::make_pair(gameObjectID, inItem);

	auto result = mItems.insert(newItem);
	return result.second;
}

bool Inventory::UpdateItem(const AItem& inItem)
{
	const int64 gameObjectID = inItem.GetGameObjectID();
	auto findItem = mItems.find(gameObjectID);
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
	const int64 gameObjectID = inItem.GetGameObjectID();
	size_t result = mItems.erase(gameObjectID);
	if (result != 0)
	{
		SubItem(inItem);
		return true;
	}

	return false;
}

const AItem* Inventory::FindItem(const int64 inObjectID)
{
	auto findItem = mItems.find(inObjectID);
	if (findItem == mItems.end())
	{
		return nullptr;
	}

	return &findItem->second;
}

const AItem* Inventory::FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY)
{
	for (auto& item : mItems)
	{
		const AItem& tempItem = item.second;
		if (tempItem.mItemCode == inItemCode && tempItem.mInventoryPositionX == inInventoryPositionX && tempItem.mInventoryPositionY == inInventoryPositionY)
		{
			return &tempItem;
		}
	}

	return nullptr;
}

bool Inventory::RollBackItem()
{
	wprintf(L"ROLLBACK ITEM\n");
	return true;
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
