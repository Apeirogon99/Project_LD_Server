#pragma once
#include "SkillTree.h"

enum class EGameDataType : uint8
{
	Item,
	ItemEqipment,
	BaseStat,
	GrowStat,
	EnemyStat,
	EnemySpawner,
	Obstruction,
	Level,
	Skill,
	MAX_GAME_DATA,
};

enum class EDataType : uint8
{
	Float,
	Integer,
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
	void LoadLevelDatas(std::map<int32, int32>& outDatas);
	void LoadSkillDatas(std::vector<SkillTreeInfo>& outSkillDatas);

public:
	bool				GetStats(const EGameDataType inDataType, const int32 inRow, Stats& outStats);
	const Stats&		GetCharacterBaseStat(const int32 inRow);
	const Stats&		GetCharacterGrowStat(const int32 inRow);
	const Stats&		GetEnemyStat(const int32 inRow);
	const Stats&		GetEqipmentStat(const int32 inRow);
	const int32			GetNextExperience(const int32& inLevel);
	const SkillTreeInfo& GetSkillInfo(const int32& inSkillID);

private:
	std::vector<Stats> mCharacterBaseStats;
	std::vector<Stats> mCharacterGrowStats;
	std::vector<Stats> mEnemyStats;
	std::vector<Stats> mEqipmentStats;
	std::vector<SkillTreeInfo> mSkillDatas;
	std::map<int32, int32> mLevelDatas;
};

