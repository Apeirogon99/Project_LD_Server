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
 
    if (false == PushData(L"P:\\Project_LD_Server\\Database\\game\\csv\\item_datas.csv"))
    {
        return false;
    }

    return true;
}
