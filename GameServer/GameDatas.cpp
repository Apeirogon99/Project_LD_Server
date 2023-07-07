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

    LoadDatas();
    return true;
}

void GameDatas::LoadDatas()
{
    LoadStatsDatas(mEqipmentStats, EGameDataType::ItemEqipment);
    LoadStatsDatas(mCharacterBaseStats, EGameDataType::BaseStat);
    LoadStatsDatas(mCharacterGrowStats, EGameDataType::GrowStat);
    LoadStatsDatas(mEnemyStats, EGameDataType::EnemyStat);
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
