#include "pch.h"
#include "Dungeon.h"

Dungeon::Dungeon() : GameObject(L"Dungeon"), mState(EDungeonState::State_Stop), mIsCreateStage(false), mStageCount(0)
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


}

void Dungeon::OnDestroy()
{
}

void Dungeon::OnTick(const int64 inDeltaTime)
{
	if(false == this->IsValid())
	{
		return;
	}

	if (false == mIsCreateStage)
	{
		bool result = this->IsCreateStage(mStageCount))
		if(result)
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
	bool player = mPlayers.size() > 0;
	bool state	= mState == EDungeonState::State_Play;
	return player && state;
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

}

bool Dungeon::ConditionStageB()
{

}

bool Dungeon::ConditionBossStage()
{

}

void Dungeon::CreateStageA()
{
	mEnemys.clear();

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
	Stats stats = datas->GetEnemyStat(static_cast<int32>(EnemyID::Enemy_Nomal_Skeleton));

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

		mEnemys.push_back(newEnemy);
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
