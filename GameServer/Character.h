#pragma once
class Character
{
public:
	Character(const RemotePlayerRef& inReomtePlayer);
	virtual ~Character();

public:
	//virtual void Init(const int32 inCharacterID, const Protocol::SCharacterData& inCharacterData, const Protocol::STransform& inTransform);
	//virtual void Destroy();
	//virtual bool IsValid();

public:
	void UpdateWorldTransform(Protocol::STransform& inWorldPosition);

public:
	Protocol::SCharacterData& GetCharacterData();

private:
	RemotePlayerRef				mRemotePlayer;

	int32						mCharacterID;
	Protocol::SCharacterData	mCharacterData;
	Protocol::STransform		mWorldPosition;
};

