#pragma once

enum class EGameDataType : uint8
{
	Item,
	ItemEqipment,
	BaseStat,
	GrowStat,
	EnemyStat,
	MAX_GAME_DATA
};

class GameDatas : public DataManager
{
public:
	GameDatas(const int32 inMaxDatas);
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

