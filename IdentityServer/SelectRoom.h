#pragma once
class SelectRoom : public GameObject
{
public:
	SelectRoom(WorldRef inWorld);
	virtual ~SelectRoom();

protected:
	virtual void Initialization() override;
	virtual void Destroy() override;
	virtual void Tick() override;
	virtual bool IsValid() override;

public:
	void LoadCharacters(PlayerStatePtr inPlayerState, Protocol::C2S_LoadCharacters inPacket);
	void StartCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_StartGame inPacket);
	void DeleteCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_DeleteCharacter inPacket);

protected:
	const ERoomType GetRoomType() { return ERoomType::SelectRoom; }

private:
	WorldRef mWorld;
};
