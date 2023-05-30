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

class AItem : public Actor
{
public:
	AItem(const int64 inGameObjectID);
	AItem(const int64 inGameObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation);
	virtual ~AItem();

	AItem(const AItem& inItem);
	AItem(const Protocol::SItem& inItem);

	AItem& operator=(const AItem& inItem);
	AItem& operator=(const Protocol::SItem& inItem);

private:
	AItem(AItem&& inItem) = delete;
	AItem(Protocol::SItem&& inItem) = delete;

	AItem& operator=(AItem&& inItem) = delete;
	AItem& operator=(Protocol::SItem&& inItem) = delete;

public:
	virtual void Initialization() override;
	virtual void Destroy() override;
	virtual void Tick() override;
	virtual bool IsValid() override;

public:
	void Init(const Protocol::SItem& inItem);

public:
	int32 mItemCode;
	int32 mInventoryPositionX;
	int32 mInventoryPositionY;
	int32 mRotation;
};

