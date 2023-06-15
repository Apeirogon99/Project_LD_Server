#pragma once

static std::wstring serverName = L"project-ld-database.csvni51jms4v.ap-northeast-2.rds.amazonaws.com";
static ADOConnectionInfo CommonAccountDatabaseInfo(L"SQLOLEDB", serverName.c_str(), L"account_database", L"SSPI", L"NO", L"admin", L"PRLD23!dev", EDBMSTypes::AWS_MSSQL);
static ADOConnectionInfo CommonGameDatabaseInfo(L"SQLOLEDB", serverName.c_str(), L"game_database", L"SSPI", L"NO", L"admin", L"PRLD23!dev", EDBMSTypes::AWS_MSSQL);
static ADOConnectionInfo CommonSharedDatabaseInfo(L"SQLOLEDB", serverName.c_str(), L"shared_database", L"SSPI", L"NO", L"admin", L"PRLD23!dev", EDBMSTypes::AWS_MSSQL);

enum class EDatabase
{
	Account,
	Game,
	Shared,
};

class IdentityDatabase : public DatabaseManager
{
public:
	IdentityDatabase(const uint32 inThreadPoolSize, const uint32 poolSize);
	virtual ~IdentityDatabase();

	IdentityDatabase(const IdentityDatabase&) = delete;
	IdentityDatabase(IdentityDatabase&&) = delete;
	IdentityDatabase& operator=(const IdentityDatabase&) = delete;
	IdentityDatabase& operator=(IdentityDatabase&&) = delete;

protected:
	virtual void InitializeDatabase() override;

private:

};

