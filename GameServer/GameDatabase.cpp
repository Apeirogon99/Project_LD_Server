#include "pch.h"
#include "GameDatabase.h"

GameDatabase::GameDatabase(const size_t poolSize) : DatabaseManager(poolSize)
{
}

GameDatabase::~GameDatabase()
{
}

void GameDatabase::InitializeDatabase()
{
	ADOConnectionInfo accountDatabaseInfo(L"SQLOLEDB", L"APEIROGON", L"account_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnectionInfo gameDatabaseInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);

	ADOConnection accountConnection;
	ADOConnection gameConnection;

	PushConnectionPool(accountConnection, accountDatabaseInfo);
	PushConnectionPool(gameConnection, gameDatabaseInfo);
}
