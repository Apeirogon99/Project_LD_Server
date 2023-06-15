#pragma once
class RemotePlayer : public GameObject
{
public:
	RemotePlayer();
	virtual ~RemotePlayer();

public:
	virtual void		Initialization()	override;
	virtual void		Destroy()			override;
	virtual void		Tick()				override;
	virtual bool		IsValid()			override;

public:
	void				SetRoomType(const ERoomType inRoomType);
	void				SetGlobalID(const int32 inGlobalID);
	void				SetServerID(const int32 inServerID);
	void				SetToken(const std::string& inToken);
	void				SetCharacters(const std::vector<CharacterPtr>& inCharacters);

public:
	const ERoomType&	GetRoomType()	{ return mRoomType; }
	const std::string&	GetToken()		{ return mToken; }
	const int32&		GetGlobalID()	{ return mGlobalID; }
	const int32&		GetServerID()	{ return mServerID; }

	CharacterPtr		GetCharacter(const int8* inName);
	CharacterPtr		GetCharacter(const int32 inSeat);

private:
	ERoomType	mRoomType;
	std::string	mToken;
	int32		mGlobalID;
	int32		mServerID;

	std::vector<CharacterPtr> mCharacters;
};

