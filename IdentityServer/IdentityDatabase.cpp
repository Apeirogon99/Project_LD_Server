#include "pch.h"
#include "IdentityDatabase.h"

IdentityDatabase::IdentityDatabase(const uint32 inThreadPoolSize, const uint32 poolSize) : DatabaseManager(inThreadPoolSize, poolSize)
{
}

IdentityDatabase::~IdentityDatabase()
{
}

void IdentityDatabase::InitializeDatabase()
{
	ADOConnection accountConnection;
	ADOConnection gameConnection;
	ADOConnection sharedConnection;

	PushConnectionPool(accountConnection, CommonAccountDatabaseInfo);
	PushConnectionPool(gameConnection, CommonGameDatabaseInfo);
	PushConnectionPool(sharedConnection, CommonSharedDatabaseInfo);
}
