#pragma once

class Inventory : public GameObject
{
public:
	Inventory(const RemotePlayerRef& inReomtePlayer, const int32 inInventoryWidth, const int32 inInventoryHeight);
	~Inventory();

public:
	virtual void Initialization()	override;
	virtual void Destroy()			override;
	virtual void Tick()				override;
	virtual bool IsValid()			override;

public:
	void LoadItemToInventory(Protocol::C2S_LoadInventory inPacket);
	void InsertItemToInventory(Protocol::C2S_InsertInventory inPacket);
	void UpdateItemToInventory(Protocol::C2S_UpdateInventory inPacket);
	void DeleteItemToInventory(Protocol::C2S_DeleteInventory inPacket);

	void UpdateEqipment(Protocol::C2S_UpdateEqipment inPacket);
	void DeleteEqipment(Protocol::C2S_DeleteEqipment inPacket);

public:
	bool LoadItem(Protocol::S2C_LoadInventory& inPacket);

	bool InsertItem(const AItemPtr& inItem);
	bool UpdateItem(const AItemPtr& inItem);
	bool DeleteItem(const AItemPtr& inItem);

	bool FindItem(const int64 inObjectID, AItemPtr& outItem);
	bool FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY, AItemPtr& outItem);

	bool RollBackItem();

	bool CheckInventory();

public:
	bool	GetItemRow(const int32 inItemCode, CSVRow& outRow);
	CSVRow* PeekItemRow(const int32 inItemCode);

protected:
	bool AddItem(const AItemPtr& item);
	bool SubItem(const AItemPtr& item);

	void PrintInventoryDebug();

private:
	bool								mIsLoad;

	int32								mWidth;
	int32								mHeight;
	int32								mStorage;
	uint8*								mInventory;
	std::unordered_map<int64, AItemPtr>	mItems;

	RemotePlayerRef						mRemotePlayer;
};

