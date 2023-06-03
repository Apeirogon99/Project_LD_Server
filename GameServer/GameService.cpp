#include "pch.h"
#include "GameService.h"

using namespace std;

GameService::GameService()
{
	SocketUtils::Init();
	PacketHandler::Init();

std::wstring dumpPath = L"";
#if NETWORK_LOCAL
dumpPath = L"P:\\Project_LD_Server\\Dump\\GameServer";
#else
dumpPath = L"C:\\ProjectLDServer\\Project_LD_Server\\Dump\\GameServer";
#endif

WinDump::Init(dumpPath.c_str());

}

GameService::~GameService()
{
	SocketUtils::Clear();
}

bool GameService::SettingService()
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
	const SessionFactory sessionFactory = std::make_shared<PlayerState>;
	const uint32 maxSessionCount = 10;
	const uint32 maxBufferSize = 0x1000;
	GameStatePtr gameState = std::make_shared<GameState>(sessionFactory, maxSessionCount, maxBufferSize);
	SessionManagerPtr sessionManager = ::static_pointer_cast<SessionManager>(move(gameState));
	if (false == SetSessionManager(sessionManager))
	{
		return false;
	}

std::wstring ip = L"";
#if NETWORK_LOCAL
ip = L"192.168.123.112";
#else
ip = L"125.180.66.59";
#endif

	//Listener
	IPAddressPtr IdentityIpAddress = std::make_shared<IPAddress>();
	IdentityIpAddress->SetIp(ip.c_str(), 10000, EProtocolType::IPv4);
	//IdentityIpAddress->SetPort(9000);

	GameListenerPtr identityListener = std::make_shared<GameListener>(IdentityIpAddress);
	ListenerPtr listener = ::static_pointer_cast<Listener>(move(identityListener));
	if (false == SetListener(listener))
	{
		return false;
	}

	//Database(ADO)
	GameDatabasePtr Gamedatabase = std::make_shared<GameDatabase>(1, 10);
	DatabaseManagerPtr  databaseManager = ::static_pointer_cast<DatabaseManager>(move(Gamedatabase));
	if (false == SetDatabaseManager(databaseManager))
	{
		return false;
	}

	//Data(CSV)
	GameDatasPtr gameDatas = std::make_shared<GameDatas>();
	DataManagerPtr  dataManager = ::static_pointer_cast<DataManager>(move(gameDatas));
	if (false == SetDataManager(dataManager))
	{
		return false;
	}

std::wstring logPath = L"";
#if NETWORK_LOCAL
logPath = L"P:\\Project_LD_Server\\Logger\\GameServer\\GameServer.log";
#else
logPath = L"C:\\ProjectLDServer\\Project_LD_Server\\Logger\\GameServer\\GameServer.log";
#endif

	//Logger
	const WCHAR* LoggerName = L"GameServer";
	const WCHAR* filePath = logPath.c_str();
	ELogMode LogMode = ELogMode::Both;
	LoggerManagerPtr Logger = std::make_shared<LoggerManager>(LoggerName, logPath.c_str(), LogMode);
	if (false == SetLoggerManager(Logger))
	{
		return false;
	}

	return true;
}