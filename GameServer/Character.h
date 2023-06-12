#pragma once
class Character : public Pawn
{
public:
	Character(const RemotePlayerRef& inReomtePlayer);
	virtual ~Character();

public:
	virtual void Initialization()	override;
	virtual void Destroy()			override;
	virtual void Tick()				override;
	virtual bool IsValid()			override;

public:
	virtual void AppearActor(PlayerStatePtr inAppearPlayerState) override;
	virtual void DisAppearActor(PlayerStatePtr inDisappearPlayerState) override;

public:
	void MoveDestination(Protocol::C2S_MovementCharacter inPakcet);

public:
	void SetCharacterData(Protocol::SCharacterData inCharacterData);

public:
	int32						GetCharacterID() { return mCharacterID; }
	Protocol::SCharacterData&	GetCharacterData() { return mCharacterData; }

private:
	RemotePlayerRef				mRemotePlayer;

	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;

	Protocol::SVector			mOldLocation;
	int64						mLastMovementTimeStamp;
};

