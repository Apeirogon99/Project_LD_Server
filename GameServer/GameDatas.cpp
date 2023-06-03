#include "pch.h"
#include "GameDatas.h"

GameDatas::GameDatas()
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

    if (false == PushData((dataPath + L"item_datas.csv").c_str()))
    {
        return false;
    }

    return true;
}
