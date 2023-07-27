#pragma once
class EqipmentComponent
{
public:
	EqipmentComponent();
	~EqipmentComponent();

	EqipmentComponent(const EqipmentComponent&) = delete;
	EqipmentComponent(EqipmentComponent&&) noexcept = delete;

	EqipmentComponent& operator=(const EqipmentComponent&) = delete;
	EqipmentComponent& operator=(EqipmentComponent&&) noexcept = delete;

public:
	void UpdateEqipmentStats(ActorPtr inActor, Inventoryptr inInventory);
	void UpdateEqipment(Protocol::SCharacterData& inCharacterData, const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart);

	void LoadEqipment(google::protobuf::RepeatedPtrField<Protocol::SItem>* inEqipments);
	void CreateEqipment(TaskManagerPtr inTask, const int32& inItemCode, const int32& inPart);
	bool InsertEqipment(Inventoryptr inInventory, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart);
	bool DeleteEqipment(Inventoryptr inInventory, const AItemPtr& inInsertInventoryItem, const Protocol::ECharacterPart& inPart);
	bool ReplaceEqipment(Inventoryptr inInventory, const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart);

public:
	const std::vector<AItemPtr>&	GetEqipments() const;
	const Stats&					GetEqipmentStats() const;
	const int32						GetEqipmentPartCode(Protocol::ECharacterPart inPart) const;

private:
	std::vector<AItemPtr>	mEqipments;
	Stats					mEqipmentStats;
};

