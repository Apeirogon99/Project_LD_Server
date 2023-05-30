#include "pch.h"
#include "Actor.h"

Actor::Actor(const char* inClassName, const int64 inGameObjectID) : mActorName(nullptr), mGameObjectID(inGameObjectID)
{
	SetActorName(inClassName);
}

Actor::Actor(const WCHAR* inActorName, const int64 inGameObjectID) : mActorName(nullptr), mGameObjectID(inGameObjectID)
{
	
}

Actor::~Actor()
{
	mGameObjectID = 0;
}

bool Actor::IsValid()
{
	return (mGameObjectID != 0);
}

void Actor::SetOwner(RemotePlayerRef inOwner)
{
	mOwner = inOwner;
}

void Actor::SetLocation(const float inX, const float inY, const float inZ)
{
	mLocation.set_x(inX);
	mLocation.set_y(inY);
	mLocation.set_z(inZ);
}

void Actor::SetLocation(const Protocol::SVector& inLocation)
{
	mLocation = inLocation;
}

void Actor::SetVelocity(const Protocol::SVector& inVelocity)
{
	mVelocity = inVelocity;
}

void Actor::SetRotator(const Protocol::SRotator& inRotator)
{
	mRotator = inRotator;
}

const WCHAR* Actor::GetActorName() const
{
	return mActorName;
}

const int64 Actor::GetGameObjectID() const
{
	return mGameObjectID;
}

RemotePlayerRef Actor::GetOwner() const
{
	return mOwner;
}

const Protocol::SVector& Actor::GetLocation() const
{
	return mLocation;
}

const Protocol::SVector& Actor::GetVelocity() const
{
	return mVelocity;
}

const Protocol::SRotator& Actor::GetRotator() const
{
	return mRotator;
}

void Actor::SetActorName(const int8* inClassName)
{
	WCHAR wcsbuf[32];
	//swprintf_s(wcsbuf, 32, L"[%ws:%lld]", inClassName, mGameObjectID);
	mActorName = wcsbuf;
}
