#include "pch.h"
#include "Dungeon.h"

Dungeon::Dungeon() : GameWorld(L"Dungeon"), mDungeonID(0), mState(EDungeonState::State_Stop), mIsCreateStage(false), mStageCount(0), mPlayerStart(2000.0f, 2000.0f, 486.0f)
{
	mConditionStageFunc.push_back(&Dungeon::ConditionStageA);
	mConditionStageFunc.push_back(&Dungeon::ConditionStageB);
	mConditionStageFunc.push_back(&Dungeon::ConditionBossStage);

	mCreateStageFunc.push_back(&Dungeon::CreateStageA);
	mCreateStageFunc.push_back(&Dungeon::CreateStageB);
	mCreateStageFunc.push_back(&Dungeon::CreateBossStage);

	mCheckStateFunc.push_back(&Dungeon::CheackStageA);
	mCheckStateFunc.push_back(&Dungeon::CheackStageB);
	mCheckStateFunc.push_back(&Dungeon::CheackBossStage);

	mClearStateFunc.push_back(&Dungeon::ClearStageA);
	mClearStateFunc.push_back(&Dungeon::ClearStageB);
	mClearStateFunc.push_back(&Dungeon::ClearBossStage);
}

Dungeon::~Dungeon()
{
}

void Dungeon::OnInitialization()
{
	mState = EDungeonState::State_Stop;

	this->ResetDungeon();
}

void Dungeon::OnDestroy()
{
}

void Dungeon::OnTick(const int64 inDeltaTime)
{
	if (false == this->IsValid())
	{
		return;
	}

	if (false == mIsCreateStage)
	{
		bool result = this->IsCreateStage(mStageCount);
		if (result)
		{
			mIsCreateStage = true;
		}
	}

	if (true == mIsCreateStage)
	{
		bool result = this->IsCheckStage(mStageCount);
		if (result)
		{
			mIsCreateStage = false;
		}
	}

}

bool Dungeon::IsValid()
{
	bool player = mWorldPlayers.size() > 0;
	bool state = mState == EDungeonState::State_Play;
	return player && state;
}

void Dungeon::SetDungeonID(int32 inDungeonID)
{
	mDungeonID = inDungeonID;
}

void Dungeon::CreateDungeon(PlayerStatePtr inPlayerState)
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
	const int64& inRemoteID = remotePlayer->GetGameObjectID();

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return;
	}

	if (false == party->IsParty())
	{
		party->CreateParty();
	}

	EDCommonErrorType error = EDCommonErrorType::FAILURE;
	if (true == party->IsLeader(inRemoteID))
	{
		error = EDCommonErrorType::SUCCESS;
	}

	Protocol::S2C_ResponseEnterDungeon enterPacket;
	std::string level = "L_NecromancerDungeon";
	enterPacket.set_level(level);
	enterPacket.set_dungeon_id(this->mDungeonID);
	enterPacket.set_error(static_cast<int32>(error));

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, enterPacket);
	party->PartyBroadCast(sendBuffer);
}

void Dungeon::CompleteLoadDungeon(PlayerStatePtr inPlayerState)
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
	const int64& remoteID = remotePlayer->GetGameObjectID();

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return;
	}

	GameWorldPtr previousWorld = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == previousWorld)
	{
		return;
	}
	previousWorld->LevelTravel(std::static_pointer_cast<GameWorld>(this->GetWorldPtr()), remoteID);

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return;
	}

	Location randomLocation = Random::GetRandomVectorInRange2D(mPlayerStart, 300.0f);
	character->GetMovementComponent().SetNewDestination(character->GetActorPtr(), randomLocation, randomLocation, previousWorld->GetWorldTime(), 0.0f);
	remotePlayer->OnLoadComplete();
	
	int32 max = static_cast<int32>(party->GetPartyPlayers().size());
	int32 cur = static_cast<int32>(mWorldPlayers.size());
	if (max == cur)
	{
		Protocol::S2C_CompleteLoadDungeon completePacket;

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, completePacket);
		this->SendWorldPlayers(sendBuffer);

		this->mState = EDungeonState::State_Play;
	}
	else
	{
		Protocol::S2C_WaitingLoadDungeon waitingPacket;
		waitingPacket.set_max_number(static_cast<int32>(max));
		waitingPacket.set_least_number(cur);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, waitingPacket);
		this->SendWorldPlayers(sendBuffer);
	}
}

void Dungeon::ResetDungeon()
{
	mState = EDungeonState::State_Ready;
}

bool Dungeon::IsCreateStage(int32 inStageCount)
{
	//스테이지 조건
	auto conditionFunc = mConditionStageFunc[inStageCount];
	bool result = conditionFunc(*this);
	if (false == result)
	{
		return false;
	}

	//스테이지 생성
	auto createFunc = mCreateStageFunc[inStageCount];
	createFunc(*this);

	return result;
}

bool Dungeon::ConditionStageA()
{
	return false;
}

bool Dungeon::ConditionStageB()
{
	return false;
}

bool Dungeon::ConditionBossStage()
{
	return false;
}

void Dungeon::CreateStageA()
{
	WorldPtr world = std::static_pointer_cast<World>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == datas)
	{
		return;
	}
	const Stats& stats = datas->GetEnemyStat(static_cast<int32>(EnemyID::Enemy_Nomal_Skeleton));

	std::vector<Location> locations;
	locations.push_back(Location());
	locations.push_back(Location());
	locations.push_back(Location());
	locations.push_back(Location());
	locations.push_back(Location());

	for (const Location& location : locations)
	{
		ActorPtr newActor = world->SpawnActor<EnemyNomalSkeleton>(this->GetGameObjectRef(), location, Rotation(), Scale(1.0f, 1.0f, 1.0f));
		std::shared_ptr<EnemyNomalSkeleton> newEnemy = std::static_pointer_cast<EnemyNomalSkeleton>(newActor);
		if (nullptr == newEnemy)
		{
			return;
		}
		newEnemy->SetEnemeyID(static_cast<int32>(EnemyID::Enemy_Nomal_Skeleton));
		newEnemy->SetActorType(static_cast<uint8>(EActorType::Enemy));
		newEnemy->SetEnemyStats(stats);
		newEnemy->SetRecoveryLocation(location);
		newEnemy->SetAggressive(true);
		newEnemy->SetSpawnObjectID(this->GetGameObjectID());
		newEnemy->GetStateManager().SetState(EStateType::State_Search);
		newEnemy->SetReward(false);
	}

}

void Dungeon::CreateStageB()
{
}

void Dungeon::CreateBossStage()
{
}

bool Dungeon::IsCheckStage(int32 inStageCount)
{
	//스테이지 중간 상황 체크
	auto cheackFunc = mCheckStateFunc[inStageCount];
	bool result = cheackFunc(*this);
	if (false == result)
	{
		return false;
	}

	//스테이지 클리어시
	auto clearFunc = mClearStateFunc[inStageCount];
	clearFunc(*this);

	return result;
}

bool Dungeon::CheackStageA()
{
	return false;
}

bool Dungeon::CheackStageB()
{
	return false;
}

bool Dungeon::CheackBossStage()
{
	return false;
}

void Dungeon::ClearStageA()
{

}

void Dungeon::ClearStageB()
{

}

void Dungeon::ClearBossStage()
{

}

bool Dungeon::IsReady() const
{
	return mState == EDungeonState::State_Ready;
}

bool Dungeon::IsPlay() const
{
	return mState == EDungeonState::State_Play;
}
