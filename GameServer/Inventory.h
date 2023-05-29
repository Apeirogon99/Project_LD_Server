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

class Item
{
public:
	Item();
	Item(const int64 inObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInventoryPositionX, const int32 inInventoryPositionY, const int32 inRotation);
	Item(const Protocol::SItem* inItem);
	~Item();

	Item(const Item& inOtherItem);
	Item(Item&& inOtherItem) noexcept;

	Item& operator=(const Item& inOtherItem);
	Item& operator=(Item&& inOtherItem) noexcept;

public:
	bool IsValid();

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

class Inventory
{
public:
	Inventory(const RemotePlayerRef& inReomtePlayer, const int32 inInventoryWidth, const int32 inInventoryHeight);
	~Inventory();

public:
	bool PushItem(const int64 inObjectID, const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInventoryPositionX, const int32 inInventoryPositionY, const int32 inRotation);
	bool PushItem(const Protocol::SItem* inItem);
	bool PushItem(const Item& inItem);
	bool LoadItem(std::unordered_map<int64, Item>& inItems);
	bool UpdateItem(const Item& inItem);
	bool DeleteItem(const Item& inItem);
	bool FindItem(const int64 inObjectID, Item& outItem);
	bool FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY, Item& outItem);
	bool CheckInventory();

public:
	bool GetItemRow(const int32 inItemCode, CSVRow& outRow);

protected:
	bool AddItem(const Item& item);

private:
	int32							mWidth;
	int32							mHeight;
	int32							mStorage;
	uint8*							mInventory;
	std::unordered_map<int64, Item>	mItems;

	RemotePlayerRef					mRemotePlayer;
};

