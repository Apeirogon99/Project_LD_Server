#pragma once
class GameListener : public Listener
{
public:
	GameListener(IPAddressPtr& ipAddr);
	virtual ~GameListener();

private:
	GameListener(const GameListener&) = delete;
	GameListener(GameListener&&) = delete;
	GameListener& operator=(const GameListener&) = delete;
	GameListener& operator=(GameListener&&) = delete;

public:


private:

};

