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
};

