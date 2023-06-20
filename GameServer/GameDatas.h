#pragma once

enum class EGameDataType : uint8
{
	Item,
	BaseStat,
	GrowStat,
};

class GameDatas : public DataManager
{
public:
	GameDatas();
	virtual ~GameDatas();

private:
	GameDatas(const GameDatas&) = delete;
	GameDatas(GameDatas&&) = delete;
	GameDatas& operator=(const GameDatas&) = delete;
	GameDatas& operator=(GameDatas&&) = delete;

public:
	virtual bool InitDatas() override;

private:

};

