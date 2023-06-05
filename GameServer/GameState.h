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
	GameTaskPtr GetGameTask();
};

