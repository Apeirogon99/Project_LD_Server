#include "pch.h"
#include "GameService.h"

using namespace std;

GameService::GameService()
{
	SocketUtils::Init();
	PacketHandler::Init();
	WinDump::Init(L"P:\\Project_LD_Server\\Builds\\Dump\\");
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
	const uint32 maxBufferSize = 0xfff;
	GameStatePtr gameState = std::make_shared<GameState>(sessionFactory, maxSessionCount, maxBufferSize);
	SessionManagerPtr sessionManager = ::static_pointer_cast<SessionManager>(move(gameState));
	if (false == SetSessionManager(sessionManager))
	{
		return false;
	}

	//Listener
	IPAddressPtr IdentityIpAddress = std::make_shared<IPAddress>();
	IdentityIpAddress->SetIp(L"192.168.123.112", 10000, EProtocolType::IPv4);
	//IdentityIpAddress->SetPort(9000);

	GameListenerPtr identityListener = std::make_shared<GameListener>(IdentityIpAddress);
	ListenerPtr listener = ::static_pointer_cast<Listener>(move(identityListener));
	if (false == SetListener(listener))
	{
		return false;
	}

	//Database(ADO)
	GameDatabasePtr identitydatabase = std::make_shared<GameDatabase>(1, 10);
	DatabaseManagerPtr  databaseManager = ::static_pointer_cast<DatabaseManager>(move(identitydatabase));
	if (false == SetDatabaseManager(databaseManager))
	{
		return false;
	}

	//Logger
	const WCHAR* LoggerName = L"GameServer";
	const WCHAR* filePath = L"P:\\Project_LD_Server\\Logger\\GameServer\\GameServer.log";
	ELogMode LogMode = ELogMode::Console;
	LoggerManagerPtr Logger = std::make_shared<LoggerManager>(LoggerName, LogMode);
	if (false == SetLoggerManager(Logger))
	{
		return false;
	}

	return true;
}