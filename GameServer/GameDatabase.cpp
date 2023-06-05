#include "pch.h"
#include "GameDatabase.h"
#include <ConnectionInfo.h>

GameDatabase::GameDatabase(const uint32 inThreadPoolSize, const uint32 poolSize) : DatabaseManager(inThreadPoolSize, poolSize)
{
}

GameDatabase::~GameDatabase()
{
}

void GameDatabase::InitializeDatabase()
{

	ADOConnection accountConnection;
	ADOConnection gameConnection;
	ADOConnection sharedConnection;

	PushConnectionPool(accountConnection, CommonAccountDatabaseInfo);
	PushConnectionPool(gameConnection, CommonGameDatabaseInfo);
	PushConnectionPool(sharedConnection, CommonSharedDatabaseInfo);

}
