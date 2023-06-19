#pragma once
class Token
{
public:
	Token();
	~Token();

	Token(const Token& inToken);
	Token& operator=(const Token& inToken) noexcept;

	Token(Token&& inToken);
	Token& operator=(Token&& inToken) noexcept;

public:
	bool CompareToken(const std::string& inToken);

public:
	void SetToken(const std::string& inToken);
	void SetGlobalID(const int32 inGlobalID);
	void SetCharacterID(const int32 inCharacterID);
	void SetLastTick(const int64 inLastTick);

public:
	const std::string&	GetToken()			const { return mToken; }
	const int32			GetGlobalID()		const { return mGlobalID; }
	const int32			GetCharacterID()	const { return mCharacterID; }
	const int64			GetLastTick()		const { return mLastTick; }

private:
	std::string mToken;
	int32		mGlobalID;
	int32		mCharacterID;
	int64		mLastTick;
};

