#include "pch.h"
#include "GameService.h"

using namespace std;

GameService::GameService()
{
	SocketUtils::Init();
	PacketManager::Init();

std::wstring dumpPath = L"";
#if NETWORK_LOCAL
dumpPath = L"P:\\Project_LD_Server\\Dump\\GameServer\\";
#else
dumpPath = L"C:\\ProjectLDServer\\Project_LD_Server\\Dump\\GameServer\\";
#endif

WinDump::Init(dumpPath.c_str());

}

GameService::~GameService()
{
	SocketUtils::Clear();
}

bool GameService::SettingService()
{

	std::wstring ip = L"";
	int32 port;
#if NETWORK_LOCAL
	//ip = L"192.168.123.112";
	ip = L"127.0.0.1";
	port = 10000;
#else
	//ip = L"125.180.66.59";
	ip = L"127.0.0.1";
	port = 10000;
#endif

	//IOCPServer
	IOCPServerPtr IocpServer = std::make_shared<IOCPServer>();
	if (false == SetIOCPServer(IocpServer))
	{
		return false;
	}

	//ThreadManager
	const uint32 MaxThreadCount = 1;
	const uint32 MaxTimeOut = INFINITE;
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
	gameState->SetServerID(2);
	gameState->SetServerName("Å¸·é");
	gameState->SetServerIP("116.41.116.247");
	gameState->SetServerPort(port);
	gameState->SetServerLevel("L_Kingdom");

	SessionManagerPtr sessionManager = ::static_pointer_cast<SessionManager>(move(gameState));
	if (false == SetSessionManager(sessionManager))
	{
		return false;
	}

	//Listener
	IPAddressPtr gameIpAddress = std::make_shared<IPAddress>();
	gameIpAddress->SetIp(ip.c_str(), port, EProtocolType::IPv4);
	//IdentityIpAddress->SetPort(9000);

	GameListenerPtr gameListener = std::make_shared<GameListener>(gameIpAddress);
	ListenerPtr listener = ::static_pointer_cast<Listener>(move(gameListener));
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
	GameDatasPtr gameDatas = std::make_shared<GameDatas>(static_cast<int32>(EGameDataType::MAX_GAME_DATA));
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

	GameTaskPtr gameTask = std::make_shared<GameTask>();
	TaskManagerPtr taskMnager = ::static_pointer_cast<TaskManager>(gameTask);
	if (false == SetTaskManager(taskMnager))
	{
		return false;
	}

	return true;
}