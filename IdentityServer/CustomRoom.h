#pragma once
class CustomRoom : public GameObject
{
public:
	CustomRoom();
	virtual ~CustomRoom();

protected:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void CreateCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_CreateCharacter inPacket);

protected:
	const ERoomType GetRoomType() { return ERoomType::CustomRoom; }
};

