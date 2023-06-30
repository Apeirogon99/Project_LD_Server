#pragma once
class Character : public Pawn
{
public:
	Character(const RemotePlayerRef& inReomtePlayer);
	virtual ~Character();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void AppearActor(PlayerStatePtr inAppearPlayerState) override;
	virtual void DisAppearActor(PlayerStatePtr inDisappearPlayerState) override;

public:
	void SyncLocation(const int64 inDeltaTime);
	virtual void OnMovement(const int64 inTimeStamp) override;

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
};

