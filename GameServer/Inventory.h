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
	void SetLoad(bool inIsLoad);
	void CreateItem(const int32 inItemCode, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation);
	void CreateEqipment(const int32 inItemCode, const int32 inPart);

public:
	void LoadItemToInventory(Protocol::C2S_LoadInventory inPacket);
	void InsertItemToInventory(Protocol::C2S_InsertInventory inPacket);
	void UpdateItemToInventory(Protocol::C2S_UpdateInventory inPacket);
	void DeleteItemToInventory(Protocol::C2S_DeleteInventory inPacket);
	void ReplcaeItemToEqipment(Protocol::C2S_ReplaceEqipment inPacket);

	bool LoadItem(google::protobuf::RepeatedPtrField<Protocol::SItem>* inItems);
	bool LoadEqipment(google::protobuf::RepeatedPtrField<Protocol::SItem>* inEqipments);
public:
	bool InsertItem(const AItemPtr& inItem);
	bool UpdateItem(const AItemPtr& inItem);
	bool DeleteItem(const AItemPtr& inItem);

	bool InsertEqipment(const AItemPtr& inInsertInventoryItem, const Protocol::ECharacterPart& inPart);
	bool DeleteEqipment(const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart);
	bool ReplaceEqipment(const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart);

	bool FindItem(const int64 inObjectID, AItemPtr& outItem);
	bool FindItem(const int32 inItemCode, const int32 inInventoryPositionX, const int32 inInventoryPositionY, AItemPtr& outItem);
	bool IsValidItem(const int64 inObjectID);

	bool RollBackItem();
	bool CheckInventory();

public:
	bool	IsLoad() { return mIsLoad; }
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
	std::vector<AItemPtr>				mEqipments;

	RemotePlayerRef						mRemotePlayer;
};

