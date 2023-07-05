#pragma once
class GameRemotePlayer : public RemotePlayer
{
public:
	GameRemotePlayer();
	~GameRemotePlayer();

private:
	GameRemotePlayer(const GameRemotePlayer&) = delete;
	GameRemotePlayer(GameRemotePlayer&&) = delete;

	GameRemotePlayer& operator=(const GameRemotePlayer&) = delete;
	GameRemotePlayer& operator=(GameRemotePlayer&&) = delete;

public:
	virtual void	OnInitialization()				override;
	virtual void	OnDestroy()						override;
	virtual void	OnTick(const int64 inDeltaTime)	override;
	virtual bool	IsValid()						override;

public:
	bool LoadRemotePlayer(const Token& inToken, GameWorldRef inWorld);
	void OnLoadComplete();

public:
	WorldRef			GetWorld()	{ return mWorld; }
	Inventoryptr&		GetInventory()	{ return mInventory; }
	PlayerCharacterPtr&	GetCharacter()	{ return mPlayerCharacter; }

private:
	Token				mToken;
	WorldRef			mWorld;

	Inventoryptr		mInventory;
	PlayerCharacterPtr	mPlayerCharacter;
};