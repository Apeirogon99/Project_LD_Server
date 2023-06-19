#include "pch.h"
#include "Token.h"

Token::Token() : mToken(""), mGlobalID(0), mCharacterID(0), mLastTick(0)
{
}

Token::~Token()
{
}

Token::Token(const Token& inToken)
{
	this->mToken		= inToken.mToken;
	this->mGlobalID		= inToken.mGlobalID;
	this->mCharacterID	= inToken.mCharacterID;
	this->mLastTick		= inToken.mLastTick;
}

Token& Token::operator=(const Token& inToken) noexcept
{
	this->mToken		= inToken.mToken;
	this->mGlobalID		= inToken.mGlobalID;
	this->mCharacterID	= inToken.mCharacterID;
	this->mLastTick		= inToken.mLastTick;
	return *this;
}

Token::Token(Token&& inToken)
{
	this->mToken		= inToken.mToken;
	this->mGlobalID		= inToken.mGlobalID;
	this->mCharacterID	= inToken.mCharacterID;
	this->mLastTick		= inToken.mLastTick;
}

Token& Token::operator=(Token&& inToken) noexcept
{
	this->mToken		= inToken.mToken;
	this->mGlobalID		= inToken.mGlobalID;
	this->mCharacterID	= inToken.mCharacterID;
	this->mLastTick		= inToken.mLastTick;
	return *this;
}

bool Token::CompareToken(const std::string& inToken)
{
	return (this->mToken.compare(inToken) == 0);
}

void Token::SetToken(const std::string& inToken)
{
	mToken = inToken;
}

void Token::SetGlobalID(const int32 inGlobalID)
{
	mGlobalID = inGlobalID;
}

void Token::SetCharacterID(const int32 inCharacterID)
{
	mCharacterID = inCharacterID;
}

void Token::SetLastTick(const int64 inLastTick)
{
	mLastTick = inLastTick;
}
