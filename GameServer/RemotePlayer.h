#pragma once
class RemotePlayer : public GameObject
{
public:
	//RemotePlayer(const PlayerStateRef& inPlayerState, const int64 inRemoteID);
	RemotePlayer(const PlayerStateRef& inPlayerState);
	~RemotePlayer();

private:
	RemotePlayer(const RemotePlayer&) = delete;
	RemotePlayer(RemotePlayer&&) = delete;

	RemotePlayer& operator=(const RemotePlayer&) = delete;
	RemotePlayer& operator=(RemotePlayer&&) = delete;

public:
	virtual void	Initialization()	override;
	virtual void	Destroy()			override;
	virtual void	Tick()				override;
	virtual bool	IsValid()			override;

public:
	bool LoadRemotePlayer(const int32 inServerID, const int32 inCharacterID);
	void LoadComplete();

	void InitTask(GameTaskPtr& inGameTask);
	void DestroyTask(GameTaskPtr& inGameTask);

public:
	void BrodcastViewers(SendBufferPtr inSendBuffer);

public:
	RemotePlayerRef GetRemotePlayer()			{ return std::static_pointer_cast<RemotePlayer>(shared_from_this()); }	    
	PlayerStateRef	GetPlayerState()	const	{ return mPlayerState; }
	Inventoryptr&	GetInventory()				{ return mInventory; }
	CharacterPtr&	GetCharacter()				{ return mCharacter; }
	Viewers&		GetViewers()				{ return mViewers; }
	ViewActors&		GetViewActors()				{ return mViewActors; }

private:
	PlayerStateRef	mPlayerState;
	Viewers			mViewers;
	ViewActors		mViewActors;

	Inventoryptr	mInventory;
	CharacterPtr	mCharacter;
};