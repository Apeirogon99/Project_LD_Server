#include "pch.h"
#include "DungeonManager.h"

DungeonManager::DungeonManager() : Actor(L"DungeonManager")
{
}

DungeonManager::~DungeonManager()
{
}

void DungeonManager::OnInitialization()
{
}

void DungeonManager::OnDestroy()
{
}

void DungeonManager::OnTick(const int64 inDeltaTime)
{
	
}

bool DungeonManager::IsValid()
{
	return mDungeons.size();
}

void DungeonManager::RequestEnterDungeon(GameRemotePlayerPtr inGameRemotePlayer)
{
	if (nullptr == inGameRemotePlayer)
	{
		return;
	}


}
