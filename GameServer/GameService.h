#pragma once
class GameService : public Service
{
public:
	GameService();
	virtual ~GameService();

	GameService(const GameService& service) = delete;
	GameService& operator=(const GameService& service) = delete;

protected:
	virtual bool		SettingService() override;

public:
	int32 GetServerID() { return mServerID; }

private:
	int32 mServerID;
};

