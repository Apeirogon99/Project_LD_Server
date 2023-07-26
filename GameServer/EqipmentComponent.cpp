#include "pch.h"
#include "EqipmentComponent.h"

EqipmentComponent::EqipmentComponent()
{
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

	GameDatasPtr dataManager = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == dataManager)
	{
		return;
	}

	mEqipmentStats.Clear();

	const std::vector<AItemPtr>& eqipments = inInventory->GetEqipments();
	for (AItemPtr eqipment : eqipments)
	{
		const Stats& eqipmentStats = dataManager->GetEqipmentStat(eqipment->GetItemCode());
		//mEqipmentStats += eqipmentStats;
	}
}
