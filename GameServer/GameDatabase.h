#pragma once
class GameDatabase : public DatabaseManager
{
public:
	GameDatabase(const size_t inThreadPoolSize, const size_t poolSize);
	virtual ~GameDatabase();

	GameDatabase(const GameDatabase&) = delete;
	GameDatabase(GameDatabase&&) = delete;
	GameDatabase& operator=(const GameDatabase&) = delete;
	GameDatabase& operator=(GameDatabase&&) = delete;

protected:
	virtual void InitializeDatabase() override;

private:
};

