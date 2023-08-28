#include "pch.h"
#include "IdentityService.h"

using namespace std;

IdentityService::IdentityService()
{
	SocketUtils::Init();
	PacketHandler::Init();

	std::wstring dumpPath = L"";
#if NETWORK_LOCAL
	dumpPath = L"P:\\Project_LD_Server\\Dump\\Identity\\";
#else
	dumpPath = L"C:\\ProjectLDServer\\Project_LD_Server\\Dump\\Identity\\";
#endif

	WinDump::Init(dumpPath.c_str());
}

IdentityService::~IdentityService()
{
	SocketUtils::Clear();
}

void IdentityService::OnServiceOpen()
{
	std::vector<Protocol::SServerInfo> gameServerInfo;
	Handle_LoadServerList_Requset(EServerType::GAME_SERVER, gameServerInfo);

	GameStatePtr gameState = std::static_pointer_cast<IdentityGameState>(GetSessionManager());
	for (int32 index = 0; index < gameServerInfo.size(); ++index)
	{
		const Protocol::SServerInfo& info = gameServerInfo.at(index);
		std::wstring ip;
		ip.assign(info.ip().begin(), info.ip().end());

		const int32 port = info.port();

		SessionPtr gameServerSession = gameState->CreateSession(ESessionMode::Server);

		WinSocketPtr gameServerSocket = gameServerSession->GetWinSocket();
		IPAddressPtr gameServerIPAddr = std::make_shared<IPAddress>();
		gameServerIPAddr->SetIp(ip.c_str(), port);
		gameServerSocket->Connect(gameServerIPAddr);

		gameState->InsertSession(gameServerSession);

		gameServerSession->RegisterRecv();
	}

	IdentityTaskPtr task = gameState->GetTask();
	LoginWorldPtr world = task->GetWorld();
	world->SetServerInfo(gameServerInfo);
}

void IdentityService::OnServiceClose()
{
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
	const uint32 MaxTimeOut = INFINITE;
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
#ifdef NETWORK_LOCAL
ip = L"192.168.123.112";
#else
ip = L"127.0.0.1";
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
	IdentityDataManagerPtr identityDataManager = std::make_shared<IdentityDataManager>(0);
	DataManagerPtr  dataManager = ::static_pointer_cast<DataManager>(move(identityDataManager));
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
	const WCHAR* LoggerName = L"IdentityServer";
	const WCHAR* filePath = logPath.c_str();
	ELogMode LogMode = ELogMode::Console;
	LoggerManagerPtr Logger = std::make_shared<LoggerManager>(LoggerName, LogMode);
	if (false == SetLoggerManager(Logger))
	{
		return false;
	}

	IdentityTaskPtr task = std::make_shared<IdentityTask>();
	TaskManagerPtr taskManager = ::static_pointer_cast<TaskManager>(task);
	if (false == SetTaskManager(taskManager))
	{
		return false;
	}

	return true;
}