#include "pch.h"
#include "Inventory.h"

Inventory::Inventory(const int32 inInventoryWidth, const int32 inInventoryHeight) : GameObject(L"Inventory"), mIsLoad(false), mWidth(inInventoryWidth), mHeight(inInventoryHeight), mStorage(0), mInventory(nullptr), mMoney(0)
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

void Inventory::SetLoadInventory(bool inIsLoad)
{
	mIsLoad = inIsLoad;
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

	PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(remotePlayer->GetCharacter());
	if (nullptr == character)
	{
		return;
	}

	EqipmentComponent& eqipment = character->GetEqipmentComponent();

	Protocol::S2C_LoadInventory loadInventoryPacket;

	this->LoadItem(loadInventoryPacket.mutable_item());
	eqipment.LoadEqipment(loadInventoryPacket.mutable_eqipment());
	loadInventoryPacket.set_money(this->mMoney);
	loadInventoryPacket.set_error(ErrorToInt(EGameErrorType::SUCCESS));

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, loadInventoryPacket);
	playerState->Send(sendBuffer);
}

void Inventory::InsertItemToInventory(const int64 inGameObjectID, const int32 inItemCode, const Location inLocation, Protocol::SVector2D inInvenPosition, const int32 inRotation, const int32 inAmount)
{
	//printf("[  ITEM   ]\n");
	//printf("[OBJECT ID] %lld\n", inGameObjectID);
	//printf("[ITEM CODE] %d\n", inItemCode);
	//printf("[INVEN POS] (%d, %d)\n", inInvenPosition.x(), inInvenPosition.y());
	//printf("[INVEN ROT] %d\n", inRotation);
	//printf("[WORLD POS] (%f, %f, %f)\n", inLocation.GetX(), inLocation.GetY(), inLocation.GetZ());
	//printf("[ AMOUNT  ] %d\n", inAmount);

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

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());

	ActorPtr itemActor;
	if (false == world->FindActor(inGameObjectID, itemActor))
	{
		InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::INVALID_ACTOR_IN_WORLD));
	}
	else
	{
		PlayerCharacterPtr	character			= remotePlayer->GetCharacter();
		Location			characterLocation	= character->GetLocation();
		Location			itemLocation		= itemActor->GetLocation();

		float distance = FVector::Distance2D(characterLocation, itemLocation);
		if (distance >= 100.0f)
		{
			InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::ACTOR_FAR_FROM_CHARACTER));

			character->GetMovementComponent().SetNewDestination(character->GetActorPtr(), characterLocation, itemLocation, world->GetWorldTime(), 100.0f);
			character->SetPlayerMode(EPlayerMode::PickUp_MODE);
			character->SetTargetActor(itemActor->GetActorRef());
			character->OnMovement();
			return;
		}

		if (InsertInventoryPacket.error() == 0)
		{
			GameTaskPtr task = std::static_pointer_cast<GameTask>(world->GetTaskManagerRef().lock());
			if (inItemCode == 171)
			{
				UpdateMoney(inAmount);
				Handle_UpdateMoney_Requset(packetSession, inAmount);
				InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::SUCCESS));
			}
			else
			{
				AItemPtr newItem = std::make_shared<AItem>();
				GameObjectPtr itemGameObject = newItem->GetGameObjectPtr();
				task->CreateGameObject(itemGameObject);

				const int32					characterID = remotePlayer->GetCharacter()->GetCharacterID();

				newItem->Init(inItemCode, inInvenPosition.x(), inInvenPosition.y(), inRotation);
				bool insertResult = InsertItem(newItem);
				if (insertResult)
				{
					Handle_InsertInventory_Requset(packetSession, characterID, newItem->GetItemCode(), newItem->GetInventoryPosition(), newItem->GetInventoryRoation());

					if (false == world->DestroyActor(inGameObjectID))
					{
						InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::INSERT_ERROR));
					}

					InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::SUCCESS));
				}
				else
				{
					InsertInventoryPacket.set_error(ErrorToInt(EGameErrorType::INSERT_ERROR));
					task->DestroyGameObject(itemGameObject);
				}
			}
		}
	}

	InsertInventoryPacket.set_remote_id(remotePlayer->GetGameObjectID());
	InsertInventoryPacket.set_object_id(inGameObjectID);

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
		const Protocol::SVector2D oldInventoryPosition = oldItem->GetInventoryPosition();
		bool updateResult = UpdateItem(oldItem, updateItem.inven_position());
		if (updateResult)
		{

			Handle_UpdateInventory_Requset(packetSession, remotePlayer->GetCharacter()->GetCharacterID(), updateItem.item_code(), oldInventoryPosition, updateItem.inven_position(), oldItem->GetInventoryRoation());

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
				
				ActorPtr newActor = world->SpawnActor<AItem>(this->GetOwner(), newLocation, newRotation, newScale);
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

	Inventoryptr&		inventory	= remotePlayer->GetInventory();
	PlayerCharacterPtr& character	= remotePlayer->GetCharacter();
	EqipmentComponent&	eqipment	= character->GetEqipmentComponent();

	const Protocol::ECharacterPart& part = inPacket.part();
	AItemPtr insertInventoryItem	= std::make_shared<AItem>(inPacket.insert_inven_item());
	AItemPtr insertEqipmentItem		= std::make_shared<AItem>(inPacket.insert_eqip_item());

	bool isValidEqipment	= false;
	bool isValidInventory	= false;
	bool isReplace			= false;

	isValidEqipment	 = (eqipment.GetEqipmentPartCode(part) == insertInventoryItem->GetItemCode()) && insertInventoryItem->GetItemCode() != 0;
	isValidInventory = inventory->IsValidItem(insertEqipmentItem->GetGameObjectID());

	if (false == isValidEqipment && true == isValidInventory)
	{
		eqipment.InsertEqipment(inventory, insertEqipmentItem, part);
	}
	else if (true == isValidEqipment && false == isValidInventory)
	{
		eqipment.DeleteEqipment(inventory, insertInventoryItem, part);
	}
	else if (true == isValidEqipment && true == isValidInventory)
	{
		eqipment.ReplaceEqipment(inventory, insertInventoryItem, insertEqipmentItem, part);
	}

	eqipment.UpdateEqipment(character->GetCharacterData(), insertInventoryItem, insertEqipmentItem, part);
	eqipment.UpdateEqipmentStats(character, inventory);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	Handle_ReplaceEqipment_Requset(packetSession, insertInventoryItem, insertEqipmentItem, part);
	

	Protocol::S2C_ReplaceEqipment replaceEqipmentPacket;
	replaceEqipmentPacket.set_remote_id(remotePlayer->GetGameObjectID());
	replaceEqipmentPacket.mutable_eqipment()->CopyFrom(character->GetCharacterData().eqipment());
	replaceEqipmentPacket.set_error(isReplace);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, replaceEqipmentPacket);
	remotePlayer->BrodcastPlayerViewers(sendBuffer);
}

bool Inventory::LoadItem(google::protobuf::RepeatedPtrField<Protocol::SItem>* inItems)
{
	for (auto& item : mItems)
	{
		const AItemPtr& curItem = item.second;

		const int64 objectID					= curItem->GetGameObjectID();
		const int32 itemCode					= curItem->GetItemCode();
		const Protocol::SVector& worldLocation	= PacketUtils::ToSVector(curItem->GetLocation());
		const Protocol::SVector2D& inventoryPos = curItem->GetInventoryPosition();
		const int32 inventoryRotation			= curItem->GetInventoryRoation();

		Protocol::SItem* loadItem = inItems->Add();
		loadItem->set_object_id(objectID);
		loadItem->set_item_code(itemCode);
		loadItem->mutable_world_position()->CopyFrom(worldLocation);
		loadItem->mutable_inven_position()->CopyFrom(inventoryPos);
		loadItem->set_rotation(inventoryRotation);
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
		if (mInventory[index] > 1)
		{
			return false;
		}
	}

	return true;
}

void Inventory::UpdateMoney(const int32& inMoeny)
{
	mMoney += inMoeny;
}

void Inventory::TempStartPack()
{

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int32	characterID = remotePlayer->GetCharacter()->GetCharacterID();

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);

	{
		std::vector<AItemPtr> tempItems;
		for (auto iter = mItems.begin(); iter != mItems.end(); iter++)
		{
			tempItems.push_back(iter->second);
		}

		for (auto iter = tempItems.begin(); iter != tempItems.end(); iter++)
		{
			DeleteItem(*iter);
		}

		remotePlayer->GetCharacter()->GetEqipmentComponent().ClearEqipment();
	}
	Handle_ClearInventory_Request(packetSession, characterID);

}

void Inventory::UpdateTempStartPack()
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int32	characterID = remotePlayer->GetCharacter()->GetCharacterID();

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}
	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);

	std::vector<int32> startPacksCode;
	startPacksCode.push_back(31);
	startPacksCode.push_back(51);
	startPacksCode.push_back(71);
	startPacksCode.push_back(91);
	startPacksCode.push_back(111);
	startPacksCode.push_back(131);
	startPacksCode.push_back(151);
	startPacksCode.push_back(181);
	startPacksCode.push_back(191);

	std::vector<int32> startPacksX;
	startPacksX.push_back(0);
	startPacksX.push_back(2);
	startPacksX.push_back(2);
	startPacksX.push_back(0);
	startPacksX.push_back(0);
	startPacksX.push_back(5);
	startPacksX.push_back(5);
	startPacksX.push_back(7);
	startPacksX.push_back(9);

	std::vector<int32> startPacksY;
	startPacksY.push_back(0);
	startPacksY.push_back(0);
	startPacksY.push_back(2);
	startPacksY.push_back(2);
	startPacksY.push_back(4);
	startPacksY.push_back(0);
	startPacksY.push_back(2);
	startPacksY.push_back(0);
	startPacksY.push_back(0);

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	GameTaskPtr task = std::static_pointer_cast<GameTask>(world->GetTaskManagerRef().lock());

	for (int32 i = 0; i < startPacksCode.size(); ++i)
	{
		AItemPtr newItem = std::make_shared<AItem>();
		GameObjectPtr itemGameObject = newItem->GetGameObjectPtr();
		task->CreateGameObject(itemGameObject);

		newItem->Init(startPacksCode[i], startPacksX[i], startPacksY[i], 0);

		bool insertResult = InsertItem(newItem);
		if (insertResult)
		{
			Handle_InsertInventory_Requset(packetSession, characterID, newItem->GetItemCode(), newItem->GetInventoryPosition(), newItem->GetInventoryRoation());
		}
	}

	{
		Protocol::S2C_LoadInventory loadInventoryPacket;

		this->LoadItem(loadInventoryPacket.mutable_item());
		remotePlayer->GetCharacter()->GetEqipmentComponent().LoadEqipment(loadInventoryPacket.mutable_eqipment());
		loadInventoryPacket.set_money(this->mMoney);
		loadInventoryPacket.set_error(ErrorToInt(EGameErrorType::SUCCESS));

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, loadInventoryPacket);
		playerState->Send(sendBuffer);
	}

	{
		Protocol::S2C_StartPack packet;
		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, packet);
		playerState->Send(sendBuffer);
	}
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

const int32& Inventory::GetMoney() const
{
	return mMoney;
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
	printf("[MONEY : %d]\n", this->mMoney);
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
