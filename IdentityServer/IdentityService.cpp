#include "pch.h"
#include "IdentityService.h"

using namespace std;

IdentityService::IdentityService()
{
	SocketUtils::Init();
	PacketHandler::Init();
	WinDump::Init(L"P:\\Project_LD_Server\\Builds\\Dump\\");
}

IdentityService::~IdentityService()
{
	SocketUtils::Clear();
}

bool IdentityService::SettingService()
{
	//IOCPServer
	IOCPServerPtr IocpServer = std::make_shared<IOCPServer>();
	if (false == SetIOCPServer(IocpServer))
	{
		return false;
	}

	//ThreadManager
	const uint32 MaxThreadCount = 4;
	const uint32 MaxTimeOut = 100;
	ThreadManagerPtr threadManager = std::make_shared<ThreadManager>(MaxThreadCount, MaxTimeOut);
	if (false == SetThreadManager(threadManager))
	{
		return false;
	}

	//SessionManager
	const SessionFactory sessionFactory = std::make_shared<IdentityPlayerState>;
	const uint32 maxSessionCount = 10;
	const uint32 maxBufferSize = 0xfff;
	GameStatePtr GameState = std::make_shared<IdentityGameState>(sessionFactory, maxSessionCount, maxBufferSize);
	SessionManagerPtr sessionManager = ::static_pointer_cast<SessionManager>(move(GameState));
	if (false == SetSessionManager(sessionManager))
	{
		return false;
	}

std::wstring ip;
#ifdef _DEBUG
ip = L"192.168.123.112";
#else
ip = L"52.78.191.130";
#endif

	//Listener
	IPAddressPtr IdentityIpAddress = std::make_shared<IPAddress>();
	IdentityIpAddress->SetIp(ip.c_str(), 9000, EProtocolType::IPv4);
	//IdentityIpAddress->SetPort(9000);

	IdentityListenerPtr identityListener = std::make_shared<IdentityListener>(IdentityIpAddress);
	ListenerPtr listener = ::static_pointer_cast<Listener>(move(identityListener));
	if (false == SetListener(listener))
	{
		return false;
	}

	//Database(ADO)
	IdentityDatabasePtr identitydatabase = std::make_shared<IdentityDatabase>(1, 10);
	DatabaseManagerPtr  databaseManager = ::static_pointer_cast<DatabaseManager>(move(identitydatabase));
	if (false == SetDatabaseManager(databaseManager))
	{
		return false;
	}

	//Data(CSV)
	IdentityDataManagerPtr identityDataManager = std::make_shared<IdentityDataManager>();
	DataManagerPtr  dataManager = ::static_pointer_cast<DataManager>(move(identityDataManager));
	if (false == SetDataManager(dataManager))
	{
		return false;
	}

	//Logger
	const WCHAR* LoggerName = L"IdentityServer";
	const WCHAR* filePath = L"P:\\Project_LD_Server\\Logger\\IdentityServer\\IdentityServer.log";
	ELogMode LogMode = ELogMode::Console;
	LoggerManagerPtr Logger = std::make_shared<LoggerManager>(LoggerName, LogMode);
	if (false == SetLoggerManager(Logger))
	{
		return false;
	}

	return true;
}