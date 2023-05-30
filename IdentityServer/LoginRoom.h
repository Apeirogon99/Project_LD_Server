#pragma once

class LoginRoom : public GameObject
{
public:
	LoginRoom(const SessionManagerRef& inSessionManager, const WCHAR* inLoginRoomName);
	virtual ~LoginRoom();

protected:
	virtual void Initialization() override;
	virtual void Destroy() override;
	virtual void Tick() override;
	virtual bool IsValid() override;

public:
	void Enter(PlayerStatePtr inPlayerState);
	void Leave(PlayerStatePtr inPlayerState);
	void Broadcast(PlayerStatePtr inPlayerState);

	void Signin(PlayerStatePtr inPlayerState, Protocol::C2S_Singin inPacket);
	void Signup(PlayerStatePtr inPlayerState, Protocol::C2S_Singup inPacket);
	void EmailVerified(PlayerStatePtr inPlayerState, Protocol::C2S_EmailVerified inPacket);

	void LoadCharacters(PlayerStatePtr inPlayerState, Protocol::C2S_LoadCharacters inPacket);
	void CreateCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_CreateCharacter inPacket);
	void DeleteCharacter(PlayerStatePtr inPlayerState, Protocol::C2S_DeleteCharacter inPacket);
	
protected:
	bool FindPlayer(PlayerStatePtr& inPlayerState);

private:
	int32 mPlayersCount;
	std::map<uint64, PlayerStatePtr> mPlayers;
};

