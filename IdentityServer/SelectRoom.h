#pragma once
class SelectRoom : public GameObject
{
public:
	SelectRoom();
	virtual ~SelectRoom();

protected:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void LoadCharacters(PlayerStatePtr inPlayerState, Protocol::C2S_LoadCharacters inPacket);
	void DeleteCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_DeleteCharacter inPacket);

	void StartCharacterRequest(PlayerStatePtr inPlayerState, Protocol::C2S_StartGame inPacket);
	void StartCharacterRespone(PlayerStatePtr inPlayerState, Protocol::S2C_TravelServer inPacket);

protected:
	const ERoomType GetRoomType() { return ERoomType::SelectRoom; }
};

