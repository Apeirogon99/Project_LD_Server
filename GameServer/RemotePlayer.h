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
	virtual void	OnInitialization()				override;
	virtual void	OnDestroy()						override;
	virtual void	OnTick(const int64 inDeltaTime)	override;
	virtual bool	IsValid()						override;

public:
	bool LoadRemotePlayer(const Token& inToken, WorldRef inWorld);
	void LoadComplete();

	void InitTask(GameTaskPtr& inGameTask);
	void DestroyTask(GameTaskPtr& inGameTask);

public:
	void BrodcastViewers(SendBufferPtr inSendBuffer);

public:
	WorldRef		GetWorldRef()				{ return mWorld; }
	RemotePlayerRef GetRemotePlayer()			{ return std::static_pointer_cast<RemotePlayer>(shared_from_this()); }	    
	PlayerStateRef	GetPlayerState()	const	{ return mPlayerState; }
	Inventoryptr&	GetInventory()				{ return mInventory; }
	CharacterPtr&	GetCharacter()				{ return mCharacter; }
	Viewers&		GetViewers()				{ return mViewers; }

private:
	WorldRef		mWorld;

	Token			mToken;
	PlayerStateRef	mPlayerState;
	Viewers			mViewers;

	Inventoryptr	mInventory;
	CharacterPtr	mCharacter;
};