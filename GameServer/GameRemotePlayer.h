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

	void LeaveRemotePlayer();
	bool LeaveComplete();

	void PressKey(int32 inKey);
	void ReleaseKey(int32 inKey);

public:
	WorldRef				GetWorld()				{ return mWorld; }
	const Token&			GetToken()				{ return mToken; }
	Inventoryptr&			GetInventory()			{ return mInventory; }
	FriendPtr&				GetFriend()				{ return mFriend; }
	PartyPtr&				GetParty()				{ return mParty; }
	SkillPtr&				GetSkill()				{ return mSkill; }
	PlayerCharacterPtr&		GetCharacter()			{ return mPlayerCharacter; }
	KeyboardComponent&		GetKeyboardComponent()	{ return mKeyboardComponet; }

private:
	Token					mToken;
	WorldRef				mWorld;

	Inventoryptr			mInventory;
	FriendPtr				mFriend;
	PartyPtr				mParty;
	SkillPtr				mSkill;
	PlayerCharacterPtr		mPlayerCharacter;

	KeyboardComponent		mKeyboardComponet;
};