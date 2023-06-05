#pragma once
class GameTask : public TaskManager, public std::enable_shared_from_this<GameTask>
{
public:
	GameTask();
	virtual ~GameTask();

private:
	GameTask(const GameTask& player) = delete;
	GameTask(GameTask&& player) = delete;
	GameTask& operator=(const GameTask& player) = delete;
	GameTask& operator=(GameTask&& player) = delete;

public:
	WorldPtr GetWorld();

public:
	WorldPtr mWorld;
};

