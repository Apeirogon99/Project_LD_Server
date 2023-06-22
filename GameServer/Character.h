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
	void SetLoad(bool inIsLoad);
	void SetCharacterID(const int32& inCharacterID);
	void SetCharacterData(Protocol::SCharacterData inCharacterData);
	void SetOldLocation(const Protocol::SVector& inOldLocation);
	void ReplaceEqipment(const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart);
	
	void UpdateStats();

public:
	bool						IsLoad() { return mIsLoad; }
	int32						GetCharacterID() { return mCharacterID; }
	Protocol::SCharacterData&	GetCharacterData() { return mCharacterData; }
	const Protocol::SVector&	GetOldLocation() { return mOldLocation; }
	int32						GetEqipmentPartCode(Protocol::ECharacterPart inPart);

private:
	bool						mIsLoad;

	RemotePlayerRef				mRemotePlayer;

	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;

	Protocol::SVector			mOldLocation;
	int64						mLastMovementTimeStamp;


	int64						mLastAnimationTimeStamp;

	Stats						mStats;
};

