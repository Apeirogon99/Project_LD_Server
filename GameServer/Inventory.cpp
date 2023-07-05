#include "pch.h"
#include "Inventory.h"

Inventory::Inventory(const int32 inInventoryWidth, const int32 inInventoryHeight) : GameObject(L"Inventory"), mIsLoad(false), mWidth(inInventoryWidth), mHeight(inInventoryHeight), mStorage(0), mInventory(nullptr)
{

}

Inventory::~Inventory()
{
	mItems.clear();
}

void Inventory::OnInitialization()
{
	mStorage = mWidth * mHeight;
	mInventory = new uint8[mStorage]();
	memset(mInventory, 0, mStorage);

	mEqipments.resize(9);
}

void Inventory::OnDestroy()
{
	if (mInventory)
	{
		delete[] mInventory;
	}

	mWidth		= 0;
	mHeight		= 0;
	mStorage	= 0;
	mInventory	= nullptr;
}

void Inventory::OnTick(const int64 inDeltaTime)
{
}

bool Inventory::IsValid()
{
	return mIsLoad;
}

void Inventory::SetLoad(bool inIsLoad)
{
	mIsLoad = inIsLoad;
}

void Inventory::CreateEqipment(const int32 inItemCode, const int32 inPart)
{
	GameRemotePlayerPtr remotePlayer	= std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	PlayerStatePtr		playerState		= std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	GameTaskPtr			task			= std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());

	AItemPtr newItem = std::make_shared<AItem>();
	GameObjectPtr gameObject = newItem->GetGameObjectPtr();
	task->CreateGameObject(gameObject);
	newItem->Init(inItemCode, 0, 0, 0);

	mEqipments[inPart - 1] = newItem;
}

void Inventory::LoadItemToInventory(Protocol::C2S_LoadInventory inPacket)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}

	Protocol::S2C_LoadInventory loadInventoryPacket;

	this->LoadItem(loadInventoryPacket.mutable_item());
	this->LoadEqipment(loadInventoryPacket.mutable_eqipment());
	loadInventoryPacket.set_error(ErrorToInt(EGameErrorType::SUCCESS));

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, loadInventoryPacket);
	packetSession->Send(sendBuffer);
}

void Inventory::InsertItemToInventory(Protocol::C2S_InsertInventory inPacket)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Protocol::S2C_InsertInventory InsertInventoryPacket;

	const Protocol::SItem& item = inPacket.item();
	const int64 objectID = item.object_id();

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (true == world->IsValidActor(objectID))
	{
		GameTaskPtr task = std::static_pointer_cast<GameTask>(world->GetTaskManagerRef().lock());
		world->DestroyActor(objectID);

		AItemPtr newItem = std::make_shared<AItem>();
		GameObjectPtr itemGameObject = newItem->GetGameObjectPtr();
		task->CreateGameObject(itemGameObject);

		const int32					characterID = remotePlayer->GetCharacter()->GetCharacterID();
		const int32					itemCode = item.item_code();
		const Protocol::SVector2D&	inventoryPosition = item.inven_position();
		const int32					inventoryRotation = item.rotation();

		newItem->Init(itemCode, inventoryPosition.x(), inventoryPosition.y(), inventoryRotation);
		bool insertResult = InsertItem(newItem);
		if (insertResult)
		{
			Handle_InsertInventory_Requset(packetSession, characterID, itemCode, inventoryPosition, inventoryRotation);

			InsertInventoryPacket.set_remote_id(remotePlayer->GetGameObjectID());
			InsertInventoryPacket.set_object_id(objectID);
			InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::SUCCESS));
		}
		else
		{
			InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::INSERT_ERROR));
		}
	}
	else
	{
		InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::INVALID_ACTOR_IN_WORLD));
	}

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, InsertInventoryPacket);
	remotePlayer->BrodcastPlayerViewers(sendBuffer);
}

void Inventory::UpdateItemToInventory(Protocol::C2S_UpdateInventory inPacket)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Protocol::S2C_UpdateInventory updateInventoryPacket;

	const Protocol::SItem& updateItem = inPacket.item();

	AItemPtr oldItem;
	bool find = remotePlayer->GetInventory()->FindItem(updateItem.object_id(), oldItem);
	if (oldItem)
	{
		bool updateResult = UpdateItem(oldItem, updateItem.inven_position());
		if (updateResult)
		{

			Handle_UpdateInventory_Requset(packetSession, remotePlayer->GetCharacter()->GetCharacterID(), updateItem.item_code(), updateItem.inven_position(), oldItem->GetInventoryPosition(), oldItem->GetInventoryRoation());

			updateInventoryPacket.set_error(ErrorToInt(EGameErrorType::SUCCESS));
		}
		else
		{
			updateInventoryPacket.set_error(ErrorToInt(EGameErrorType::UPDATE_ERROR));
		}
	}
	else
	{
		updateInventoryPacket.set_error(ErrorToInt(EGameErrorType::INVALID_ITEM_IN_INVENTORY));
	}

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, updateInventoryPacket);
	packetSession->Send(sendBuffer);
}

void Inventory::DeleteItemToInventory(Protocol::C2S_DeleteInventory inPacket)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Protocol::S2C_DeleteInventory deleteInventoryPacket;
	deleteInventoryPacket.set_remote_id(remotePlayer->GetGameObjectID());

	const Protocol::SItem& item = inPacket.item();
	const int64 objectID = item.object_id();

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (false == world->IsValidActor(objectID))
	{

		AItemPtr deleteItem;
		bool find = FindItem(item.object_id(), deleteItem);
		if (true == find)
		{
			bool deleteResult = DeleteItem(deleteItem);
			if (deleteResult)
			{

				Location	newLocation = PacketUtils::ToFVector(item.world_position());
				Rotation	newRotation;
				Scale		newScale;

				ActorPtr newActor = world->SpawnActor<AItem>(newLocation, newRotation, newScale);
				AItemPtr newItem = std::static_pointer_cast<AItem>(newActor);
				newItem->SetItemCode(deleteItem->GetItemCode());

				Handle_DeleteInventory_Requset(packetSession, remotePlayer->GetCharacter()->GetCharacterID(), deleteItem->GetItemCode(), deleteItem->GetInventoryPosition());

				deleteInventoryPacket.mutable_item()->CopyFrom(newItem->ConvertSItem());
				deleteInventoryPacket.set_error(ErrorToInt(EGameErrorType::SUCCESS));
			}
			else
			{
				deleteInventoryPacket.mutable_item()->CopyFrom(deleteItem->ConvertSItem());
				deleteInventoryPacket.set_error(ErrorToInt(EGameErrorType::DELETE_ERROR));
			}
		}
		else
		{
			deleteInventoryPacket.set_error(ErrorToInt(EGameErrorType::INVALID_ITEM_IN_INVENTORY));
		}
	}
	else
	{
		deleteInventoryPacket.set_error(ErrorToInt(EGameErrorType::ALREADY_ACTOR_IN_WORLD));
	}

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, deleteInventoryPacket);
	remotePlayer->BrodcastPlayerViewers(sendBuffer);
}

void Inventory::ReplcaeItemToEqipment(Protocol::C2S_ReplaceEqipment inPacket)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}

	Inventoryptr&		inventory = remotePlayer->GetInventory();
	PlayerCharacterPtr& character = remotePlayer->GetCharacter();

	const Protocol::ECharacterPart& part = inPacket.part();
	AItemPtr insertInventoryItem	= std::make_shared<AItem>(inPacket.insert_inven_item());
	AItemPtr insertEqipmentItem		= std::make_shared<AItem>(inPacket.insert_eqip_item());

	bool isValidEqipment	= false;
	bool isValidInventory	= false;
	bool isReplace			= false;

	//isValidEqipment		= (character->GetEqipmentPartCode(part) == insertInventoryItem->GetItemCode()) && insertInventoryItem->GetItemCode() != 0;
	//isValidInventory	= inventory->IsValidItem(insertEqipmentItem->GetGameObjectID());

	std::cout << "Replace : [" << insertInventoryItem->GetGameObjectID() << ", " << insertInventoryItem->GetItemCode() << "] >> [" << insertEqipmentItem->GetGameObjectID() << ", " << insertEqipmentItem->GetItemCode() << "]" << std::endl;

	if (false == isValidEqipment && true == isValidInventory)
	{
		inventory->DeleteEqipment(insertEqipmentItem, part);
	}
	else if (true == isValidEqipment && false == isValidInventory)
	{
		inventory->InsertEqipment(insertInventoryItem, part);
	}
	else if (true == isValidEqipment && true == isValidInventory)
	{
		inventory->ReplaceEqipment(insertInventoryItem, insertEqipmentItem, part);
	}

	//character->ReplaceEqipment(insertInventoryItem, insertEqipmentItem, part);
	//character->UpdateCharacterStats();

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Handle_ReplaceEqipment_Requset(packetSession, insertInventoryItem, insertEqipmentItem, part);
	

	Protocol::S2C_ReplaceEqipment replaceEqipmentPacket;
	replaceEqipmentPacket.set_remote_id(remotePlayer->GetGameObjectID());
	replaceEqipmentPacket.mutable_eqipment()->CopyFrom(character->GetCharacterData().eqipment());
	replaceEqipmentPacket.set_error(isReplace);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, replaceEqipmentPacket);
	//remotePlayer->BrodcastViewers(sendBuffer);
}

bool Inventory::LoadItem(google::protobuf::RepeatedPtrField<Protocol::SItem>* inItems)
{
	for (auto& item : mItems)
	{
		const AItemPtr& curItem = item.second;

		Protocol::SItem* loadItem = inItems->Add();
		loadItem->set_object_id(curItem->GetGameObjectID());
		loadItem->set_item_code(curItem->GetItemCode());
		loadItem->mutable_world_position()->CopyFrom(PacketUtils::ToSVector(curItem->GetLocation()));
		loadItem->mutable_inven_position()->CopyFrom(curItem->GetInventoryPosition());
		loadItem->set_rotation(curItem->GetInventoryRoation());
	}
	return true;
}

bool Inventory::LoadEqipment(google::protobuf::RepeatedPtrField<Protocol::SItem>* inEqipments)
{
	for (int32 curPart = 0; curPart < 9; ++curPart)
	{
		const AItemPtr& curEqipment = mEqipments[curPart];

		Protocol::SItem* loadEqipment = inEqipments->Add();
		loadEqipment->set_object_id(curEqipment->GetGameObjectID());
		loadEqipment->set_item_code(curEqipment->GetItemCode());
	}
	return true;
}

bool Inventory::InsertItem(const AItemPtr& inItem)
{
	if (false == AddItem(inItem))
	{
		return false;
	}

	if (false == CheckInventory())
	{
		SubItem(inItem);
		return false;
	}

	const int64 gameObjectID = inItem->GetGameObjectID();
	std::pair<int64, AItemPtr> newItem = std::make_pair(gameObjectID, inItem);

	auto result = mItems.insert(newItem);
	return result.second;
}

bool Inventory::UpdateItem(const AItemPtr& inItem, const Protocol::SVector2D& inNewInventoryPosition)
{
	const int64 gameObjectID = inItem->GetGameObjectID();
	auto findItem = mItems.find(gameObjectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	const Protocol::SVector2D oldInventoryPosition = findItem->second->GetInventoryPosition();

	SubItem(findItem->second);
	findItem->second->SetInventoryPosition(inNewInventoryPosition);
	AddItem(findItem->second);

	if (false == CheckInventory())
	{
		AddItem(findItem->second);
		findItem->second->SetInventoryPosition(oldInventoryPosition);
		SubItem(findItem->second);
		return false;
	}

	return true;
}

bool Inventory::DeleteItem(const AItemPtr& inItem)
{
	const int64 gameObjectID = inItem->GetGameObjectID();
	size_t result = mItems.erase(gameObjectID);
	if (result == 0)
	{
		return false;
	}

	if (false == SubItem(inItem))
	{
		return false;
	}

	if (false == CheckInventory())
	{
		AddItem(inItem);
		return false;
	}

	return true;
}

bool Inventory::InsertEqipment(const AItemPtr& inInsertInventoryItem, const Protocol::ECharacterPart& inPart)
{
	if (false == InsertItem(inInsertInventoryItem))
	{
		return false;
	}

	const int32 part = static_cast<int32>(inPart);
	mEqipments[part - 1]->Clear();
	return true;
}

bool Inventory::DeleteEqipment(const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart)
{
	if (false == DeleteItem(inInsertEqipmentItem))
	{
		return false;
	}

	const int32 part = static_cast<int32>(inPart);
	mEqipments[part - 1] = inInsertEqipmentItem;
	return true;
}

bool Inventory::ReplaceEqipment(const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart)
{

	if (false == InsertItem(inInsertInventoryItem))
	{
		return false;
	}

	if (false == DeleteItem(inInsertEqipmentItem))
	{
		return false;
	}

	const int32 part = static_cast<int32>(inPart);
	mEqipments[part - 1] = inInsertEqipmentItem;
	return true;
}

bool Inventory::FindItem(const int64 inObjectID, AItemPtr& outItem)
{
	auto findItem = mItems.find(inObjectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	outItem = findItem->second;
	return true;
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

bool Inventory::IsValidItem(const int64 inObjectID)
{
	auto findItem = mItems.find(inObjectID);
	if (findItem == mItems.end())
	{
		return false;
	}

	return true;
}

bool Inventory::RollBackItem()
{
	wprintf(L"ROLLBACK ITEM\n");
	return true;
}

bool Inventory::CheckInventory()
{
	//PrintInventoryDebug();

	for (int32 index = 0; index < mStorage; ++index)
	{
		if (mInventory[index] > 2)
		{
			return false;
		}
	}

	return true;
}

CSVRow* Inventory::PeekItemRow(const int32 inItemCode)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return nullptr;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return nullptr;
	}

	DataManagerPtr dataManager = world->GetDatas();
	return dataManager->PeekRow(static_cast<uint8>(EGameDataType::Item), inItemCode);
}

const std::vector<AItemPtr>& Inventory::GetEqipments()
{
	return mEqipments;
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
