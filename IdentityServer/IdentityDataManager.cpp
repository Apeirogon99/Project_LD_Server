#include "pch.h"
#include "IdentityDataManager.h"

IdentityDataManager::IdentityDataManager(const int32 inMaxDatas) : DataManager(inMaxDatas)
{
}

IdentityDataManager::~IdentityDataManager()
{
}

bool IdentityDataManager::InitDatas()
{
	return true;
}
