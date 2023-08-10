#include "pch.h"
#include "GameDatas.h"

GameDatas::GameDatas(const int32 inMaxDatas) : DataManager(inMaxDatas)
{
}

GameDatas::~GameDatas()
{
}

bool GameDatas::InitDatas()
{
    std::wstring dataPath = L"";
#if NETWORK_LOCAL
    dataPath = L"P:\\Project_LD_Server\\Data\\";
#else
    dataPath = L"C:\\ProjectLDServer\\Project_LD_Server\\Data\\";
#endif

    if (false == PushData((dataPath + L"item_datas.csv").c_str(), static_cast<int32>(EGameDataType::Item)))
    {
        return false;
    }

    if (false == PushData((dataPath + L"item_eqipment.csv").c_str(), static_cast<int32>(EGameDataType::ItemEqipment)))
    {
        return false;
    }

    if (false == PushData((dataPath + L"class_base_stats.csv").c_str(), static_cast<int32>(EGameDataType::BaseStat)))
    {
        return false;
    }

    if (false == PushData((dataPath + L"class_grow_stats.csv").c_str(), static_cast<int32>(EGameDataType::GrowStat)))
    {
        return false;
    }

    if (false == PushData((dataPath + L"enemy_stats.csv").c_str(), static_cast<int32>(EGameDataType::EnemyStat)))
    {
        return false;
    }

    if (false == PushData((dataPath + L"enemy_spawner_datas.csv").c_str(), static_cast<int32>(EGameDataType::EnemySpawner)))
    {
        return false;
    }

    if (false == PushData((dataPath + L"level_datas.csv").c_str(), static_cast<int32>(EGameDataType::Level)))
    {
        return false;
    }

    if (false == PushData((dataPath + L"skill_datas.csv").c_str(), static_cast<int32>(EGameDataType::Skill)))
    {
        return false;
    }

    LoadDatas();
    return true;
}

void GameDatas::LoadDatas()
{
    LoadStatsDatas(mEqipmentStats, EGameDataType::ItemEqipment);
    LoadStatsDatas(mCharacterBaseStats, EGameDataType::BaseStat);
    LoadStatsDatas(mCharacterGrowStats, EGameDataType::GrowStat);
    LoadStatsDatas(mEnemyStats, EGameDataType::EnemyStat);

    LoadSkillDatas(mSkillDatas);

    LoadLevelDatas(mLevelDatas);
}

void GameDatas::LoadStatsDatas(std::vector<Stats>& outDatas, EGameDataType inDataType)
{
    CSVDatas datas;
    GetData(datas, static_cast<uint8>(inDataType));

    const size_t datasSize = datas.size() - 1;

    outDatas.resize(datasSize);
    for (int32 dataIndex = 0; dataIndex < datasSize; ++dataIndex)
    {

        Stats tempStat;

        CSVRow row = datas.at(dataIndex);
        for (int32 rowIndex = 0; rowIndex < row.size(); ++rowIndex)
        {
            float rowFValue = stof(row.at(rowIndex));
            tempStat.SetStats(rowIndex, rowFValue);
        }

        outDatas[dataIndex] = tempStat;
    }
}

void GameDatas::LoadLevelDatas(std::map<int32, int32>& outDatas)
{
    CSVDatas datas;
    GetData(datas, static_cast<uint8>(EGameDataType::Level));

    const size_t datasSize = datas.size() - 1;

    for (int32 dataIndex = 1; dataIndex < datasSize; ++dataIndex)
    {
        CSVRow row = datas.at(dataIndex);
        std::pair<int32, int32> level = std::make_pair(stoi(row.at(1)), stoi(row.at(2)));
        outDatas.insert(level);
    }
}

void GameDatas::LoadSkillDatas(std::vector<SkillInfo>& outSkillDtatas)
{
    CSVDatas datas;
    GetData(datas, static_cast<uint8>(EGameDataType::Skill));

    const size_t datasSize = datas.size() - 1;

    std::wstring delim = L"-";
    SkillInfo skillInfo;
    std::map<int32, int32> conditions;

    for (int32 dataIndex = 1; dataIndex < datasSize; ++dataIndex)
    {
        conditions.clear();
        skillInfo.Clear();

        CSVRow row = datas.at(dataIndex);
        const int32& id     = stoi(row.at(0));
        const int32& type   = stoi(row.at(1));
        const int32& max    = stoi(row.at(4));
        std::wstring line   = row.at(5);

        if (0 != stoi(line))
        {
            size_t pos = 0;
            while ((pos = line.find(delim)) != std::wstring::npos)
            {
                size_t mid = line.find(L".");
                int32 a = stoi(line.substr(0, mid));
                int32 b = stoi(line.substr(mid+1, pos-1));

                std::pair<int32, int32> condition = std::make_pair(a, b);
                conditions.insert(condition);

                line.erase(0, pos + delim.length());
            }
        }

        skillInfo.Init(id, type, max, conditions);
        outSkillDtatas.push_back(skillInfo);
    }

}

bool GameDatas::GetStats(const EGameDataType inDataType, const int32 inRow, Stats& outStats)
{
    bool result = true;
    switch (inDataType)
    {
    case EGameDataType::ItemEqipment:
        outStats = GetEqipmentStat(inRow);
        break;
    case EGameDataType::BaseStat:
        outStats = GetCharacterBaseStat(inRow);
        break;
    case EGameDataType::GrowStat:
        outStats = GetCharacterGrowStat(inRow);
        break;
    case EGameDataType::EnemyStat:
        outStats = GetEnemyStat(inRow);
        break;
    default:
        result = false;
        break;
    }

    return result;
}

const Stats& GameDatas::GetCharacterBaseStat(const int32 inRow)
{
    return mCharacterBaseStats.at(inRow);
}

const Stats& GameDatas::GetCharacterGrowStat(const int32 inRow)
{
    return mCharacterGrowStats.at(inRow);
}

const Stats& GameDatas::GetEnemyStat(const int32 inRow)
{
    return mEnemyStats.at(inRow);
}

const Stats& GameDatas::GetEqipmentStat(const int32 inRow)
{
    return mEqipmentStats.at(inRow);
}

const int32 GameDatas::GetNextExperience(const int32& inLevel)
{
    auto find = mLevelDatas.find(inLevel);
    if (find == mLevelDatas.end())
    {
        return -1;
    }

    return find->second;
}

const SkillInfo& GameDatas::GetSkillInfo(const int32& inSkillID)
{
    return mSkillDatas[inSkillID - 1];
}
