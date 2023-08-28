#pragma once
class LoginWorld : public World
{
public:
	LoginWorld();
	virtual ~LoginWorld();

protected:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void EnterWorld(PlayerStatePtr inPlayerState);
	void LeaveWorld(PlayerStatePtr inPlayerState);

	void SetServerInfo(const std::vector<Protocol::SServerInfo>& inServerInfo);

public:
	WorldRef		GetWorldRef();
	LoginRoomPtr	GetLoginRoom();
	SelectRoomPtr	GetSelectRoom();
	CustomRoomPtr	GetCustomRoom();

	const std::vector<Protocol::SServerInfo>& GetServerInfo();

	bool			IsValidPlayer(RemotePlayerPtr inRemotePlayer);

private:
	LoginRoomPtr	mLoginRoom;
	SelectRoomPtr	mSelectRoom;
	CustomRoomPtr	mCustomRoom;

	std::vector<Protocol::SServerInfo> mServerInfo;
	std::map<int64, LoginRemotePlayerPtr> mRemotePlayers;
};

