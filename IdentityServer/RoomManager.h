#pragma once
class RoomManager : public GameObject
{
public:
	RoomManager();
	virtual ~RoomManager();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	bool RoomCmp(const ERoomType& inRoomType);
	bool CanChanageRoom(const ERoomType& inRoomType);

	void SetRoomType(const ERoomType& inRoomType);

public:
	const ERoomType& GetRoomType() const;

private:
	ERoomType mRoomType;
};

