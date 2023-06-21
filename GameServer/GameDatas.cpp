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

    return true;
}
