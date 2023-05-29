#pragma once
class Actor : public GameObject
{
public:
	Actor(const SessionManagerRef& inSessionManager, const WorldPtr& inWorld);
	Actor(const SessionManagerRef& inSessionManager, const WCHAR* inName, const WorldPtr& inWorld);
	virtual ~Actor();

public:
	virtual bool IsValid() abstract;

private:
	WorldPtr mWorld;
};

