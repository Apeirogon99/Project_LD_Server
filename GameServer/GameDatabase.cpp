#include "pch.h"
#include "GameDatabase.h"
#include <ConnectionInfo.h>

GameDatabase::GameDatabase(const size_t inThreadPoolSize, const size_t poolSize) : DatabaseManager(inThreadPoolSize, poolSize)
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
