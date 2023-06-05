#pragma once
class Character : public GameObject
{
public:
	Character(const RemotePlayerRef& inReomtePlayer);
	virtual ~Character();

public:
	virtual void Initialization() override
	{

	};
	virtual void Destroy() override
	{

	};
	virtual void Tick() override
	{

	}
	virtual bool IsValid() override
	{
		return true;
	}

public:
	void LoadCharacter();
	void AppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState);
	void DisAppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState);
	void MoveDestination(RemotePlayerPtr inReomtePlayer, Protocol::STransform inWorldPosition);

public:
	Protocol::SCharacterData& GetCharacterData();

private:
	RemotePlayerRef				mRemotePlayer;

	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;
	Protocol::STransform		mWorldPosition;
};

