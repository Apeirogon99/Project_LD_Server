#pragma once
class GameTask : public TaskManager
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
	virtual void Init() override;

public:
	GameWorldPtr GetWorld();

public:
	GameTaskPtr GetTaskPtr();
};

