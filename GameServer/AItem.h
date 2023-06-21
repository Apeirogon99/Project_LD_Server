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
	AItem();
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
	virtual void Initialization()	override;
	virtual void Destroy()			override;
	virtual void Tick()				override;
	virtual bool IsValid()			override;

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) override;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) override;

public:
	void Clear();

	void Init(const Protocol::SItem& inItem);
	void Init(const AItem& inItem);
	void Init(const int32 inItemCode, const float inWorldPositionX, const float inWorldPositionY, const float inWorldPositionZ, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation);
	void Init(const int32 inItemCode, const int32 inInvenPositionX, const int32 inInvenPositionY, const int32 inRotation);

public:
	void SetItemCode(const int32 inItemCode);
	void SetInventoryPosition(const Protocol::SVector2D& inInventoryPosition);
	void SetInventoryRoation(const int32 inInventoryRotation);

public:
	const int32 GetItemCode()							const { return mItemCode; }
	const Protocol::SVector2D& GetInventoryPosition()	const { return mInvenPosition; }
	const int32 GetInventoryRoation()					const { return mInvenRotation; }
	const Protocol::SItem ConvertSItem();

private:
	int32				mItemCode;
	Protocol::SVector2D mInvenPosition;
	int32				mInvenRotation;
};

