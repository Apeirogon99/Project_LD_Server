#pragma once
class LoginRemotePlayer : public RemotePlayer
{
public:
	LoginRemotePlayer();
	virtual ~LoginRemotePlayer();

public:
	virtual void		OnInitialization()				override;
	virtual void		OnDestroy()						override;
	virtual void		OnTick(const int64 inDeltaTime)	override;
	virtual bool		IsValid()						override;

public:
	IdentityManagerPtr	GetIdentityManager()	{ return mIdentityManager; }
	CharacterManagerPtr GetCharacterManager()	{ return mCharacterManager; }
	RoomManagerPtr		GetRoomManager()		{ return mRoomManager; }

private:
	IdentityManagerPtr	mIdentityManager;
	CharacterManagerPtr mCharacterManager;
	RoomManagerPtr		mRoomManager;
};

