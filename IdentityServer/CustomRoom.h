#pragma once
class CustomRoom : public GameObject
{
public:
	CustomRoom(WorldRef inWorld);
	virtual ~CustomRoom();

protected:
	virtual void Initialization()				override;
	virtual void Destroy()						override;
	virtual void Tick(const int64 inDeltaTime)	override;
	virtual bool IsValid()						override;

public:
	void CreateCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_CreateCharacter inPacket);

protected:
	const ERoomType GetRoomType() { return ERoomType::CustomRoom; }

private:
	WorldRef mWorld;
};

