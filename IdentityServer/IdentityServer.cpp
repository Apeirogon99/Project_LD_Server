#include "pch.h"
#include "IdentityPacket.pb.h"

using namespace std;

void DatabaseConnectionPool()
{
	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"account_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection conn;
	conn.OpenEx(ConnectionInfo);
}

void StartServer()
{
	IdentityServicePtr service = std::make_shared<IdentityService>();

	service->ServiceOpen();

	std::string userInput;
	while (true)
	{

		std::cin >> userInput;


		if (userInput == "q")
		{
			service->ServiceClose();
			break;
		}
	}

	cout << service.use_count() << endl;

	SocketUtils::Clear();
}

int main(void)
{

	//DatabaseConnectionPool();

	StartServer();

	system("pause");

}