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
	virtual void MoveDestination(Protocol::C2S_MovementCharacter inPakcet) override;

public:
	void SetLoad(bool inIsLoad);
	void SetCharacterID(const int32& inCharacterID);
	void SetCharacterData(Protocol::SCharacterData inCharacterData);
	void ReplaceEqipment(const AItemPtr& inInsertInventoryItem, const AItemPtr& inInsertEqipmentItem, const Protocol::ECharacterPart& inPart);
	
	void UpdateStats();

public:
	bool						IsLoad() { return mIsLoad; }
	int32						GetCharacterID() { return mCharacterID; }
	Protocol::SCharacterData&	GetCharacterData() { return mCharacterData; }
	int32						GetEqipmentPartCode(Protocol::ECharacterPart inPart);

private:
	bool						mIsLoad;

	RemotePlayerRef				mRemotePlayer;

	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;

	Stats						mStats;
};

