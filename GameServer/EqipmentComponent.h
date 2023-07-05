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

public:
	Stats mEqipmentStats;
};

