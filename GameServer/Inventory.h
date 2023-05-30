#pragma once

class Inventory
{
public:
	Inventory(const RemotePlayerRef& inReomtePlayer, const int32 inInventoryWidth, const int32 inInventoryHeight);
	~Inventory();

public:
	bool LoadItem(Protocol::S2C_LoadInventory& inPacket);

	bool InsertItem(const AItem& inItem);
	bool UpdateItem(const AItem& inItem);
	bool DeleteItem(const AItem& inItem);

	const AItem* FindItem(const int64 inObjectID);
	const AItem* FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY);

	bool RollBackItem();

	bool CheckInventory();

public:
	bool	GetItemRow(const int32 inItemCode, CSVRow& outRow);
	CSVRow* PeekItemRow(const int32 inItemCode);

protected:
	bool AddItem(const AItem& item);
	bool SubItem(const AItem& item);

	void PrintInventoryDebug();

private:
	int32								mWidth;
	int32								mHeight;
	int32								mStorage;
	uint8*								mInventory;
	std::unordered_map<int64, AItem>	mItems;

	RemotePlayerRef						mRemotePlayer;
};

