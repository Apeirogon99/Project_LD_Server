#pragma once

using ADOConnections = std::pair<ADOConnection, ADOConnectionInfo>;

class Database
{
public:
	APEIROGON_API Database(const size_t poolSize = 10);
	APEIROGON_API ~Database();

protected:
	Database(const Database&) = delete;
	Database(Database&&) = delete;

	Database operator=(const Database&) = delete;
	Database operator=(Database&&) = delete;

public:
	bool Prepare(const ServicePtr& service);
	void Shutdown();

public:
	APEIROGON_API void PushConnectionPool(ADOConnection& conn, const ADOConnectionInfo& info);
	APEIROGON_API bool StartADOService();

	APEIROGON_API void PrintConnectionPoolState();

protected:
	void KeepConnection();

private:
	bool						mIsService;
	size_t						mPoolSize;
	
	ServicePtr					mService;
	std::vector<ADOConnections>	mConnections;
};

