#pragma once

class Inventory : public GameObject
{
public:
	Inventory(const int32 inInventoryWidth, const int32 inInventoryHeight);
	~Inventory();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void SetLoadInventory(bool inIsLoad);

public:
	void LoadItemToInventory(Protocol::C2S_LoadInventory inPacket);
	void InsertItemToInventory(const int64 inGameObjectID, const int32 inItemCode, const Location inLocation, Protocol::SVector2D inInvenPosition, const int32 inRotation, const int32 inAmount);
	void UpdateItemToInventory(Protocol::C2S_UpdateInventory inPacket);
	void DeleteItemToInventory(Protocol::C2S_DeleteInventory inPacket);
	void ReplcaeItemToEqipment(Protocol::C2S_ReplaceEqipment inPacket);

	bool LoadItem(google::protobuf::RepeatedPtrField<Protocol::SItem>* inItems);

public:
	bool InsertItem(const AItemPtr& inItem);
	bool UpdateItem(const AItemPtr& inItem, const Protocol::SVector2D& inNewInventoryPosition);
	bool DeleteItem(const AItemPtr& inItem);

	bool FindItem(const int64 inObjectID, AItemPtr& outItem);
	bool FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY, AItemPtr& outItem);
	bool IsValidItem(const int64 inObjectID);

	bool RollBackItem();
	bool CheckInventory();

public:
	void UpdateMoney(const int32& inMoeny);

public:
	void TempStartPack();
	void UpdateTempStartPack();

public:
	CSVRow* PeekItemRow(const int32 inItemCode);
	const int32& GetMoney() const;

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

	int32								mMoney;
	std::unordered_map<int64, AItemPtr>	mItems;
};

