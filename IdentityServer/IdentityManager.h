#pragma once
class IdentityManager : public GameObject
{
public:
	IdentityManager();
	virtual ~IdentityManager();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void SetToken(const std::string& inToken);
	void SetGlobalID(const int32& inGlobalID);
	void SetServerID(const int32& inServerID);

public:
	const std::string&	GetToken()		{ return mToken; }
	const int32&		GetGlobalID()	{ return mGlobalID; }
	const int32&		GetServerID()	{ return mServerID; }

private:
	std::string	mToken;
	int32		mGlobalID;
	int32		mServerID;
};

