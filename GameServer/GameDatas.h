#pragma once

enum class EGameDataType : uint8
{
	Item,
	ItemEqipment,
	BaseStat,
	GrowStat,
	EnemyStat,
	MAX_GAME_DATA,
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
	void LoadDatas();

	void LoadStatsDatas(std::vector<Stats>& outDatas, EGameDataType inDataType);

public:
	bool			GetStats(const EGameDataType inDataType, const int32 inRow, Stats& outStats);
	const Stats&	GetCharacterBaseStat(const int32 inRow);
	const Stats&	GetCharacterGrowStat(const int32 inRow);
	const Stats&	GetEnemyStat(const int32 inRow);
	const Stats&	GetEqipmentStat(const int32 inRow);

private:
	std::vector<Stats> mCharacterBaseStats;
	std::vector<Stats> mCharacterGrowStats;
	std::vector<Stats> mEnemyStats;
	std::vector<Stats> mEqipmentStats;
};

