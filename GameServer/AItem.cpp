#include "pch.h"
#include "AItem.h"

AItem::AItem() : Actor(L"ITEM")
{
	mItemCode			= 0;
	mInvenPosition.set_x(0);
	mInvenPosition.set_y(0);
	mInvenRotation		= 0;
	mAmount				= 0;
}

AItem::~AItem()
{
	
}

AItem::AItem(const AItem& inItem) : Actor(L"ITEM")
{
	Init(inItem);
}

AItem::AItem(const Protocol::SItem& inItem) : Actor(L"ITEM")
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

void AItem::OnInitialization()
{
	mItemCode			= 0;
	mInvenPosition.set_x(0);
	mInvenPosition.set_y(0);
	mInvenRotation		= 0;

	mCollisionComponent.SetSphereCollisione(50.0f);
}

void AItem::OnDestroy()
{
}

void AItem::OnTick(const int64 inDeltaTime)
{

}

bool AItem::IsValid()
{
	return mItemCode != NONE_ITEM;
}

void AItem::OnAppearActor(ActorPtr inAppearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inAppearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == IsValid())
	{
		return;
	}

	if (true == this->FindPlayerViewer(anotherPlayerState))
	{
		return;
	}
	this->InsertPlayerViewer(anotherPlayerState);
	anotherPlayerState->InsertActorMonitor(this->GetActorPtr());

	Protocol::S2C_AppearItem appearItemPacket;
	appearItemPacket.mutable_item()->CopyFrom(this->ConvertSItem());

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearItemPacket);
	anotherPlayerState->Send(appearItemSendBuffer);
}

void AItem::OnDisAppearActor(ActorPtr inDisappearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inDisappearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == this->FindPlayerViewer(anotherPlayerState))
	{
		return;
	}
	this->ReleasePlayerViewer(anotherPlayerState);
	anotherPlayerState->ReleaseActorMonitor(this->GetActorPtr());

	Protocol::S2C_DisAppearGameObject disAppearItemPacket;
	disAppearItemPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr , disAppearItemPacket);
	anotherPlayerState->Send(appearItemSendBuffer);
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
	mAmount				= inItem.amount();

	SetLocation(PacketUtils::ToFVector(inItem.world_position()));
}

void AItem::Init(const AItem& inItem)
{

	this->SetGameObjectID(inItem.GetGameObjectID());

	mItemCode			= inItem.mItemCode;
	mInvenPosition		= inItem.mInvenPosition;
	mInvenRotation		= inItem.mInvenRotation;
	mAmount				= inItem.mAmount;

	SetLocation(inItem.GetLocation());
}

void AItem::Init(const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation, const int32 inAmount)
{
	mItemCode			= inItemCode;
	mInvenPosition.set_x(inInvenPositionX);
	mInvenPosition.set_y(inInvenPositionY);
	mInvenRotation		= inRotation;
	mAmount				= inAmount;

	SetLocation(inWorldPositionX, inWorldPositionY, inWorldPositionZ);
}

void AItem::Init(const int32 inItemCode, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation, const int32 inAmount)
{
	mItemCode			= inItemCode;
	mInvenPosition.set_x(inInvenPositionX);
	mInvenPosition.set_y(inInvenPositionY);
	mInvenRotation		= inRotation;
	mAmount				= inAmount;
}

void AItem::PickUp(PlayerCharacterPtr inCharacter)
{
	if (nullptr == inCharacter)
	{
		return;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(inCharacter->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	Inventoryptr inventory = remotePlayer->GetInventory();
	if (nullptr == inventory)
	{
		return;
	}

	float distance = FVector::Distance2D(inCharacter->GetLocation(), this->GetLocation());
	if (distance >= 100.0f)
	{
		inCharacter->SetPlayerMode(EPlayerMode::PickUp_MODE);
		return;
	}

	inventory->InsertItemToInventory(this->GetGameObjectID(), this->GetItemCode(), this->GetLocation(), this->GetInventoryPosition(), this->GetInventoryRoation(), this->GetAmount());
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

void AItem::SetAmount(const int32& inAmount)
{
	mAmount = inAmount;
}

void AItem::ItemDebug()
{
	printf("[  ITEM   ]\n");
	printf("[OBJECT ID] %lld\n", this->GetGameObjectID());
	printf("[ITEM CODE] %d\n", this->GetItemCode());
	printf("[INVEN POS] (%d, %d)\n", this->GetInventoryPosition().x(), this->GetInventoryPosition().y());
	printf("[INVEN ROT] %d\n", this->GetInventoryRoation());
	printf("[WORLD POS] (%f, %f, %f)\n",this->GetLocation().GetX(), this->GetLocation().GetY(), this->GetLocation().GetZ());
	printf("[ AMOUNT  ] %d\n", this->GetAmount());
}

const Protocol::SItem AItem::ConvertSItem()
{
	Protocol::SItem tempItem;
	tempItem.set_object_id(this->GetGameObjectID());
	tempItem.set_item_code(this->GetItemCode());
	tempItem.mutable_inven_position()->CopyFrom(this->GetInventoryPosition());
	tempItem.set_rotation(this->GetInventoryRoation());
	tempItem.mutable_world_position()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	tempItem.set_amount(this->GetAmount());
	return tempItem;
}
