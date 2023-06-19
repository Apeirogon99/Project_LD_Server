#pragma once
class GameState : public SessionManager
{
public:
	GameState(const SessionFactory& sessionFactory, const uint32 maxSessionCount, const uint32 inMaxBufferSize);
	virtual ~GameState();

	GameState(const GameState& player) = delete;
	GameState(GameState&& player) = delete;
	GameState& operator=(const GameState& player) = delete;
	GameState& operator=(GameState&& player) = delete;

public:
	void SetServerID(const int32 inServerID);
	void SetServerName(const std::string& inServerName);
	void SetServerIP(const std::string& inServerIP);
	void SetServerPort(const int32 inPort);
	void SetServerLevel(const std::string& inServerLevel);

public:
	const int32			GetServerID() { return mServerID; }
	const std::string&	GetServerName() { return mServerName; }
	const std::string&	GetServerIP() { return mServerIP; }
	const int32			GetServerPort() { return mServerPort; }
	const std::string&	GetServerLevel() { return mServerLevel; }
	GameTaskPtr			GetGameTask();

private:
	int32		mServerID;
	std::string mServerName;
	std::string mServerIP;
	int32		mServerPort;
	std::string mServerLevel;
};

