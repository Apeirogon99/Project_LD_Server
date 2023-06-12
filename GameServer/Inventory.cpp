#include "pch.h"
#include "Inventory.h"

Inventory::Inventory(const RemotePlayerRef& inReomtePlayer, const int32 inInventoryWidth, const int32 inInventoryHeight) : GameObject(L"Inventory"), mIsLoad(false), mWidth(inInventoryWidth), mHeight(inInventoryHeight), mRemotePlayer(inReomtePlayer)
{

}

Inventory::~Inventory()
{
	mItems.clear();
}

void Inventory::Initialization()
{
	mStorage = mWidth * mHeight;
	mInventory = new uint8[mStorage]();
	memset(mInventory, 0, mStorage);
}

void Inventory::Destroy()
{
	if (mInventory)
	{
		delete[] mInventory;
	}

	mWidth		= 0;
	mHeight		= 0;
	mStorage	= 0;
	mInventory	= nullptr;
	mRemotePlayer.reset();
}

void Inventory::Tick()
{
}

bool Inventory::IsValid()
{
	return mIsLoad;
}

void Inventory::LoadItemToInventory(Protocol::C2S_LoadInventory inPacket)
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

	Protocol::S2C_LoadInventory loadInventoryPacket;
	remotePlayer->GetInventory()->LoadItem(loadInventoryPacket);

	remotePlayer->GetInventory()->CheckInventory();

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, loadInventoryPacket);
	packetSession->Send(sendBuffer);
}

void Inventory::InsertItemToInventory(Protocol::C2S_InsertInventory inPacket)
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

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Handle_InsertInventory_Requset(packetSession, inPacket);
}

void Inventory::UpdateItemToInventory(Protocol::C2S_UpdateInventory inPacket)
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

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Handle_UpdateInventory_Requset(packetSession, inPacket);
}

void Inventory::DeleteItemToInventory(Protocol::C2S_DeleteInventory inPacket)
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

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Handle_DeleteInventory_Requset(packetSession, inPacket);
}

void Inventory::UpdateEqipment(Protocol::C2S_UpdateEqipment inPacket)
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

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Handle_UpdateEqipment_Requset(packetSession, inPacket);
}

void Inventory::DeleteEqipment(Protocol::C2S_DeleteEqipment inPacket)
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

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Handle_DeleteEqipment_Requset(packetSession, inPacket);
}

bool Inventory::LoadItem(Protocol::S2C_LoadInventory& inPacket)
{
	for (auto& item : mItems)
	{
		const AItemPtr& curItem = item.second;

		Protocol::SItem* loadItem = inPacket.add_item();
		loadItem->set_object_id(curItem->GetGameObjectID());
		loadItem->set_item_code(curItem->GetItemCode());
		loadItem->mutable_world_position()->CopyFrom(curItem->GetLocation());
		loadItem->mutable_inven_position()->CopyFrom(curItem->GetInventoryPosition());
		loadItem->set_rotation(curItem->GetInventoryRoation());
	}
	inPacket.set_error(0);
	return true;
}

bool Inventory::InsertItem(const AItemPtr& inItem)
{
	if (false == AddItem(inItem))
	{
		return false;
	}

	const int64 gameObjectID = inItem->GetGameObjectID();
	std::pair<int64, AItemPtr> newItem = std::make_pair(gameObjectID, inItem);

	auto result = mItems.insert(newItem);
	return result.second;
}

bool Inventory::UpdateItem(const AItemPtr& inItem)
{
	const int64 gameObjectID = inItem->GetGameObjectID();
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

bool Inventory::DeleteItem(const AItemPtr& inItem)
{
	const int64 gameObjectID = inItem->GetGameObjectID();
	size_t result = mItems.erase(gameObjectID);
	if (result != 0)
	{
		SubItem(inItem);
		return true;
	}

	return false;
}

bool Inventory::FindItem(const int64 inObjectID, AItemPtr& outItem)
{
	auto findItem = mItems.find(inObjectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	outItem = findItem->second;
}

bool Inventory::FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY, AItemPtr& outItem)
{

	for (auto& item : mItems)
	{
		const AItemPtr& tempItem = item.second;
		if (tempItem->GetItemCode() == inItemCode && tempItem->GetInventoryPosition().x() == inInventoryPositionX && tempItem->GetInventoryPosition().y() == inInventoryPositionY)
		{
			outItem = tempItem;
		}
	}

	return false;
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

bool Inventory::AddItem(const AItemPtr& item)
{

	CSVRow* row = PeekItemRow(item->GetItemCode());
	if (nullptr == row)
	{
		return false;
	}

	int32 itemSizeX = stoi(row->at(static_cast<int32>(EItemCellType::size_x)));
	int32 itemSizeY = stoi(row->at(static_cast<int32>(EItemCellType::size_y)));

	//가로
	if (item->GetInventoryRoation() == 0)
	{
		for (int32 indexY = item->GetInventoryPosition().y(); indexY < item->GetInventoryPosition().y() + itemSizeY; ++indexY)
		{
			for (int32 indexX = item->GetInventoryPosition().x(); indexX < item->GetInventoryPosition().x() + itemSizeX; ++indexX)
			{
				const int32 indexPos = (indexX + (indexY * mWidth));
				mInventory[indexPos] += 1;
			}
		}
	}
	//세로
	else if (item->GetInventoryRoation() == 1)
	{
		for (int32 indexY = item->GetInventoryPosition().y(); indexY < item->GetInventoryPosition().y() + itemSizeX; ++indexY)
		{
			for (int32 indexX = item->GetInventoryPosition().x(); indexX < item->GetInventoryPosition().x() + itemSizeY; ++indexX)
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

bool Inventory::SubItem(const AItemPtr& item)
{
	CSVRow* row = PeekItemRow(item->GetItemCode());
	if (nullptr == row)
	{
		return false;
	}

	int32 itemSizeX = stoi(row->at(static_cast<int32>(EItemCellType::size_x)));
	int32 itemSizeY = stoi(row->at(static_cast<int32>(EItemCellType::size_y)));

	//가로
	if (item->GetInventoryRoation() == 0)
	{
		for (int32 indexY = item->GetInventoryPosition().y(); indexY < item->GetInventoryPosition().y() + itemSizeY; ++indexY)
		{
			for (int32 indexX = item->GetInventoryPosition().x(); indexX < item->GetInventoryPosition().x() + itemSizeX; ++indexX)
			{
				const int32 indexPos = (indexX + (indexY * mWidth));
				mInventory[indexPos] -= 1;
			}
		}
	}
	//세로
	else if (item->GetInventoryRoation() == 1)
	{
		for (int32 indexY = item->GetInventoryPosition().y(); indexY < item->GetInventoryPosition().y() + itemSizeX; ++indexY)
		{
			for (int32 indexX = item->GetInventoryPosition().x(); indexX < item->GetInventoryPosition().x() + itemSizeY; ++indexX)
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
