#pragma once
class World : public GameObject
{
public:
	World(IdentityTaskPtr inIdentityTask);
	virtual ~World();

protected:
	virtual void Initialization()	override;
	virtual void Destroy()			override;
	virtual void Tick()				override;
	virtual bool IsValid()			override;

public:
	void Enter(PlayerStatePtr inPlayerState);
	void Leave(PlayerStatePtr inPlayerState);

	void SetServerInfo(const std::vector<Protocol::SServerInfo>& inServerInfo);

public:
	WorldRef		GetWorldRef();
	LoginRoomPtr	GetLoginRoom();
	SelectRoomPtr	GetSelectRoom();
	CustomRoomPtr	GetCustomRoom();

	const std::vector<Protocol::SServerInfo>& GetServerInfo();

	bool			IsValidPlayer(RemotePlayerPtr inRemotePlayer);

private:
	IdentityTaskPtr mIdentityTask;
	LoginRoomPtr	mLoginRoom;
	SelectRoomPtr	mSelectRoom;
	CustomRoomPtr	mCustomRoom;

	std::vector<Protocol::SServerInfo> mServerInfo;
	std::map<int64, RemotePlayerPtr> mRemotePlayers;
};

