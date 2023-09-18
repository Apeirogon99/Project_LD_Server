#include "pch.h"
#include "DungeonManager.h"

DungeonManager::DungeonManager() : Actor(L"DungeonManager")
{
}

DungeonManager::~DungeonManager()
{
}

void DungeonManager::OnInitialization()
{
	SetTick(true, SYSTEM_TICK);

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	TaskManagerPtr taskManager = world->GetTaskManagerRef().lock();
	if (nullptr == taskManager)
	{
		return;
	}

	for (int32 index = 0; index < 1; ++index)
	{
		DungeonPtr newDungeon = std::make_shared<Dungeon>();
		newDungeon->SetDungeonID(index);
		newDungeon->SetOwner(this->GetGameObjectRef());
		taskManager->PushTask(newDungeon->GetGameObjectPtr());

		std::pair<int32, DungeonPtr> makeDungeon = std::make_pair(index, newDungeon);
		mDungeons.insert(makeDungeon);
	}
}

void DungeonManager::OnDestroy()
{
}

void DungeonManager::OnTick(const int64 inDeltaTime)
{
	for (auto& dungeon : mDungeons)
	{
		if (dungeon.second->IsPlay())
		{
			break;
		}
	}
}

bool DungeonManager::IsValid()
{
	return mDungeons.size();
}

void DungeonManager::RequestEnterDungeon(int32 inDungeonType, PlayerStatePtr inPlayerState)
{
	if (nullptr == inPlayerState)
	{
		return;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	Protocol::S2C_RequestEnterDungeon enterPacket;

	EDCommonErrorType error = EDCommonErrorType::FAILURE;
	for (auto& dungeon : mDungeons)
	{
		if (dungeon.second->IsReady())
		{
			error = EDCommonErrorType::SUCCESS;
			dungeon.second->CreateDungeon(inPlayerState);
			break;
		}
	}

	enterPacket.set_error(static_cast<int32>(error));
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, enterPacket);
	inPlayerState->Send(sendBuffer);
}

DungeonPtr DungeonManager::GetDungeon(int32 inDungeonID)
{
	auto findDungeon = mDungeons.find(inDungeonID);
	if (findDungeon == mDungeons.end())
	{
		return nullptr;
	}

	return findDungeon->second;
}
