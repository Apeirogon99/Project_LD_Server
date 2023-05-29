#include "pch.h"
#include "Actor.h"

Actor::Actor(const SessionManagerRef& inSessionManager, const WorldPtr& inWorld) : GameObject(inSessionManager), mWorld(inWorld)
{
}

Actor::Actor(const SessionManagerRef& inSessionManager, const WCHAR* inName, const WorldPtr& inWorld) : GameObject(inSessionManager, inName), mWorld(inWorld)
{
}

Actor::~Actor()
{
	mWorld.reset();
}
