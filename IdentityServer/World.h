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

public:
	WorldRef		GetWorldRef();
	LoginRoomPtr	GetLoginRoom();
	SelectRoomPtr	GetSelectRoom();
	CustomRoomPtr	GetCustomRoom();

	bool			IsValidPlayer(RemotePlayerPtr inRemotePlayer);

private:
	IdentityTaskPtr mIdentityTask;
	LoginRoomPtr	mLoginRoom;
	SelectRoomPtr	mSelectRoom;
	CustomRoomPtr	mCustomRoom;

	std::map<int64, RemotePlayerPtr> mRemotePlayers;
};

