#pragma once

static std::wstring serverName = L"project-ld-database.csvni51jms4v.ap-northeast-2.rds.amazonaws.com";
static ADOConnectionInfo CommonAccountDatabaseInfo(L"SQLOLEDB", serverName.c_str(), L"account_database", L"SSPI", L"NO", L"admin", L"PRLD23!dev", EDBMSTypes::AWS_MSSQL);
static ADOConnectionInfo CommonGameDatabaseInfo(L"SQLOLEDB", serverName.c_str(), L"game_database", L"SSPI", L"NO", L"admin", L"PRLD23!dev", EDBMSTypes::AWS_MSSQL);
static ADOConnectionInfo CommonSharedDatabaseInfo(L"SQLOLEDB", serverName.c_str(), L"shared_database", L"SSPI", L"NO", L"admin", L"PRLD23!dev", EDBMSTypes::AWS_MSSQL);

class GameDatabase : public DatabaseManager
{
public:
	GameDatabase(const size_t inThreadPoolSize, const size_t poolSize);
	virtual ~GameDatabase();

	GameDatabase(const GameDatabase&) = delete;
	GameDatabase(GameDatabase&&) = delete;
	GameDatabase& operator=(const GameDatabase&) = delete;
	GameDatabase& operator=(GameDatabase&&) = delete;

protected:
	virtual void InitializeDatabase() override;

private:
};

