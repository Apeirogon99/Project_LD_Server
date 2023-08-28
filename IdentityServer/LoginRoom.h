#pragma once

class LoginRoom : public GameObject
{
public:
	LoginRoom();
	virtual ~LoginRoom();

protected:
	virtual void OnInitialization()				override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()						override;

public:
	void Signin(PlayerStatePtr inPlayerState, Protocol::C2S_Singin inPacket);
	void Signup(PlayerStatePtr inPlayerState, Protocol::C2S_Singup inPacket);
	void EmailVerified(PlayerStatePtr inPlayerState, Protocol::C2S_EmailVerified inPacket);

	void LoadServerRequest(PlayerStatePtr inPlayerState, Protocol::C2S_LoadServer inPacket);
	void LoadServerRespon();

	void SelectServer(PlayerStatePtr inPlayerState, Protocol::C2S_SelectServer inPacket);

protected:
	const ERoomType GetRoomType() { return ERoomType::LoginRoom; }

private:
	WorldRef mWorld;
};

