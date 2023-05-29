#pragma once

enum class EItemCellType
{
	id,
	category_id,
	character_class_id,
	race_id,
	tier_id,
	name,
	descrioption,
	cost,
	level,
	size_x,
	size_y,
	icon,
	mesh
};

class AItem
{
public:
	AItem();
	AItem(const int64 inObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInventoryPositionX, const int32 inInventoryPositionY, const int32 inRotation);
	AItem(const Protocol::SItem* inItem);
	~AItem();

	AItem(const AItem& inOtherItem);
	AItem(const Protocol::SItem& inOtherItem);
	AItem(AItem&& inOtherItem) noexcept;

	AItem& operator=(const AItem& inOtherItem);
	AItem& operator=(const Protocol::SItem& inOtherItem);
	AItem& operator=(AItem&& inOtherItem) noexcept;

public:
	//virtual void Initialization() override;
	//virtual void Destroy() override;
	//virtual void Tick() override;
	//virtual bool IsValid() override;

public:
	int64 mObjectID;
	int32 mItemCode;
	float mWorldPositonX;
	float mWorldPositonY;
	float mWorldPositonZ;
	int32 mInventoryPositionX;
	int32 mInventoryPositionY;
	int32 mRotation;
};

