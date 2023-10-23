#include "pch.h"
#include "EqipmentComponent.h"

EqipmentComponent::EqipmentComponent()
{
	mEqipments.resize(9);
}

EqipmentComponent::~EqipmentComponent()
{
}

void EqipmentComponent::UpdateEqipmentStats(ActorPtr inActor, Inventoryptr inInventory)
{
	WorldPtr world = inActor->GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(inActor);
	if (nullptr == character)
	{
		return;
	}

	StatsComponent& statsComponent = character->GetStatComponent();
	Stats maxStat = statsComponent.LoadMaxStats(character);
	const Stats& curStat = statsComponent.GetCurrentStats();
	const float speed = curStat.GetMovementSpeed();

	FVector velocity = FVector(speed, speed, speed);

	GameDatasPtr dataManager = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == dataManager)
	{
		return;
	}

	mEqipmentStats.Clear();

	for (AItemPtr eqipment : mEqipments)
	{
		const Stats& eqipmentStats = dataManager->GetEqipmentStat(eqipment->GetItemCode());
		mEqipmentStats = eqipmentStats;

		maxStat += eqipmentStats;

		velocity = velocity + eqipmentStats.GetMovementSpeed();
	}

	character->SetVelocity(velocity);
}

void EqipmentComponent::UpdateEqipment(Protocol::SCharacterData& inCharacterData, const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart)
{
	Protocol::SCharacterEqipment* eqipment = inCharacterData.mutable_eqipment();
	if (nullptr == eqipment)
	{
		return;
	}

	const int32 part = static_cast<int32>(inPart);
	const int32 insertInventoryItemCode = inInsertInventoryItem->GetItemCode();
	const int32 insertEqipmentItemCode = inInsertEqipmentItem->GetItemCode();

	static std::function<int32(const int32, const int32, const int32)> compareEqipment = [](const int32 inInstalledItemCode, const int32 inCompareItemCode, const int32 inReplaceItemCode) -> int32 { return inInstalledItemCode == inCompareItemCode ? inReplaceItemCode : inInstalledItemCode; };

	switch (inPart)
	{
	case Protocol::Part_Unspecified:
		break;
	case Protocol::Part_helmet:
		eqipment->set_helmet(compareEqipment(eqipment->helmet(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Shoulders:
		eqipment->set_shoulders(compareEqipment(eqipment->shoulders(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Chest:
		eqipment->set_chest(compareEqipment(eqipment->chest(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Bracers:
		eqipment->set_bracers(compareEqipment(eqipment->bracers(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Hands:
		eqipment->set_hands(compareEqipment(eqipment->hands(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Pants:
		eqipment->set_pants(compareEqipment(eqipment->pants(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Boots:
		eqipment->set_boots(compareEqipment(eqipment->boots(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Weapon_l:
		eqipment->set_weapon_l(compareEqipment(eqipment->weapon_l(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	case Protocol::Part_Weapon_r:
		eqipment->set_weapon_r(compareEqipment(eqipment->weapon_r(), insertInventoryItemCode, insertEqipmentItemCode));
		break;
	default:
		break;
	}

}


void EqipmentComponent::LoadEqipment(google::protobuf::RepeatedPtrField<Protocol::SItem>* inEqipments)
{
	for (int32 curPart = 0; curPart < 9; ++curPart)
	{
		const AItemPtr& curEqipment = mEqipments[curPart];

		Protocol::SItem* loadEqipment = inEqipments->Add();
		loadEqipment->set_object_id(curEqipment->GetGameObjectID());
		loadEqipment->set_item_code(curEqipment->GetItemCode());
	}

}

void EqipmentComponent::CreateEqipment(TaskManagerPtr inTask, const int32& inItemCode, const int32& inPart)
{
	AItemPtr newItem = std::make_shared<AItem>();
	GameObjectPtr gameObject = newItem->GetGameObjectPtr();
	inTask->CreateGameObject(gameObject);
	newItem->Init(inItemCode, 0, 0, 0);

	mEqipments[inPart - 1] = newItem;
}

bool EqipmentComponent::InsertEqipment(Inventoryptr inInventory, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart)
{
	if (false == inInventory->DeleteItem(inInsertEqipmentItem))
	{
		return false;
	}

	const int32 part = static_cast<int32>(inPart);
	mEqipments[part - 1] = inInsertEqipmentItem;
	return true;
}

bool EqipmentComponent::DeleteEqipment(Inventoryptr inInventory, const AItemPtr& inInsertInventoryItem, const Protocol::ECharacterPart& inPart)
{
	if (false == inInventory->InsertItem(inInsertInventoryItem))
	{
		return false;
	}

	const int32 part = static_cast<int32>(inPart);
	mEqipments[part - 1]->Clear();
	return true;
}

bool EqipmentComponent::ReplaceEqipment(Inventoryptr inInventory, const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart)
{
	if (false == inInventory->InsertItem(inInsertInventoryItem))
	{
		return false;
	}

	if (false == inInventory->DeleteItem(inInsertEqipmentItem))
	{
		return false;
	}

	const int32 part = static_cast<int32>(inPart);
	mEqipments[part - 1] = inInsertEqipmentItem;
	return true;
}

void EqipmentComponent::ClearEqipment()
{
	for (auto eqipment : mEqipments)
	{
		eqipment->Clear();
	}
}

const std::vector<AItemPtr>& EqipmentComponent::GetEqipments() const
{
	return mEqipments;
}

const Stats& EqipmentComponent::GetEqipmentStats() const
{
	return mEqipmentStats;
}

const int32 EqipmentComponent::GetEqipmentPartCode(Protocol::ECharacterPart inPart) const
{
	return mEqipments[inPart - 1]->GetItemCode();
}