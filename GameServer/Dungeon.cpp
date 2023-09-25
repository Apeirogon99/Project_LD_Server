#include "pch.h"
#include "Dungeon.h"

Dungeon::Dungeon() : GameWorld(L"Dungeon"), mDungeonID(0), mState(EDungeonState::State_Stop), mStageCount(0), mPlayerStart(2000.0f, 2000.0f, 486.0f), mMaxPlayers(0)
{
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
	SetTick(true, SYSTEM_TICK);

	mState = EDungeonState::State_Stop;

	GameTaskPtr task = std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());
	if (nullptr == task)
	{
		return;
	}
	mEnemySpawnerManger = std::make_shared<EnemySpawnerManager>();
	mEnemySpawnerManger->SetOwner(this->GetGameObjectRef());
	task->PushTask(mEnemySpawnerManger->GetGameObjectPtr());

	this->mSequenceComponent.Init(std::static_pointer_cast<Dungeon>(this->shared_from_this()), &Dungeon::EndSequence);

	this->InitDungeon();
}

void Dungeon::OnDestroy()
{
	GameTaskPtr task = std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());
	if (nullptr == task)
	{
		return;
	}
	task->ReleaseTask(mEnemySpawnerManger->GetGameObjectPtr());
}

void Dungeon::OnTick(const int64 inDeltaTime)
{
	if (false == this->IsValid())
	{
		return;
	}
	const int64 nextWorldTime = this->GetNextWorldTime();

	if (true == mSequenceComponent.IsPlay())
	{
		mSequenceComponent.Update(inDeltaTime);
		return;
	}

	VisibleAreaSync(inDeltaTime);

	this->PushTask(nextWorldTime, &GameWorld::RefreshWorldObserver);

	this->IsCheckStage(mStageCount);

	this->IsDeathPlayers();

}

bool Dungeon::IsValid()
{
	bool state = mState == EDungeonState::State_Play;
	return state;
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

	GameWorldPtr previousWorld = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == previousWorld)
	{
		return;
	}
	mOriginWorld = previousWorld;
	previousWorld->LevelTravel(std::static_pointer_cast<GameWorld>(this->GetWorldPtr()), remoteID);

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return;
	}

	Location randomLocation = Random::GetRandomVectorInRange2D(mPlayerStart, 300.0f);
	character->GetMovementComponent().SetNewDestination(character->GetActorPtr(), randomLocation, randomLocation, previousWorld->GetWorldTime(), 0.0f);
	remotePlayer->OnLoadComplete();

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return;
	}
	party->BroadCastLoadParty();
	
	int32 max = static_cast<int32>(party->GetPartyPlayers().size());
	int32 cur = static_cast<int32>(mWorldPlayers.size());
	if (max == cur)
	{
		mMaxPlayers = max;
		this->mSequenceComponent.SetMaxPlayers(mMaxPlayers);

		mState = EDungeonState::State_Play;

		this->CreateBossStage();

		Protocol::S2C_CompleteLoadDungeon completePacket;

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, completePacket);
		this->SendWorldPlayers(sendBuffer);
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

void Dungeon::InitDungeon()
{
	mStageCount = 0;

	mEnemySpawnerManger->ClearEnemySpawner();

	this->mWorldObstruction.DeleteTree();
	this->mWorldObserver.DeleteTree();
	this->DestroyAllActor();
	this->mWorldPlayers.clear();

	ActorPtr newActor = this->SpawnActor<Trap<Dungeon>>(this->GetGameObjectRef(), Location(500.0f, -200.0f, 400.0f), Rotation(), Scale(1.0f, 1.0f, 1.0f));
	if (nullptr == newActor)
	{
		return;
	}

	std::shared_ptr<Trap<Dungeon>> newTrap = std::static_pointer_cast<Trap<Dungeon>>(newActor);
	if (nullptr == newTrap)
	{
		return;
	}
	newTrap->SetTrigger(true);
	newTrap->SetMaxOverlap(this->mMaxPlayers);
	newTrap->SetOverlapType(EActorType::Player);
	newTrap->SetBoxTriggerExtent(FVector(1000.0f, 1300.0f, 500.0f));
	newTrap->SetEnterTrapCallBackFunction(&Dungeon::CreateStageA);

	{
		this->MakeWorldObstruction(EGameDataType::DungeonObstruction, 6);
	}

	mState = EDungeonState::State_Ready;
}

void Dungeon::CreateStageA()
{

	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(+600.0f, -650.0f, +474.0f), 700.0f, EnemyID::Enemy_Nomal_Skeleton, 3, 1, true, true, 0.0f, 2000.0f);
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(-100.0f, +300.0f, +474.0f), 400.0f, EnemyID::Enemy_Archer_Skeleton, 2, 1, true, true, 0.0f, 2000.0f);
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(-800.0f, +600.0f, +474.0f), 500.0f, EnemyID::Enemy_Warrior_Skeleton, 1, 1, true, true, 0.0f, 2000.0f);

	{
		this->MakeWorldObstruction(EGameDataType::DungeonObstruction, 5);
	}

}

void Dungeon::CreateStageB()
{
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(-5000.0f, +600.0f, +474.0f), 700.0f, EnemyID::Enemy_Warrior_Skeleton, 4, 3, true, true, 0.0f, 5000.0f);
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(-6500.0f, +600.0f, +474.0f), 400.0f, EnemyID::Enemy_Nomal_Skeleton, 3, 3, true, true, 0.0f, 5000.0f);
	mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(-5750.0f, -200.0f, +474.0f), 500.0f, EnemyID::Enemy_Archer_Skeleton, 2, 3, true, true, 0.0f, 5000.0f);

	{
		this->MakeWorldObstruction(EGameDataType::DungeonObstruction, 7);
	}
}

void Dungeon::CreateBossStage()
{

	//TEMP
	std::shared_ptr<Portal> portal = std::static_pointer_cast<Portal>(SpawnActor<Portal>(this->GetGameObjectRef(), Location(1500.0f, 1500.0f, 500.0f), FRotator(), Scale()));
	portal->SetTrigger(true);
	portal->SetMaxOverlap(this->mMaxPlayers);
	portal->SetSphereTriggerRadius(300.0f);
	portal->SetWaitTeleprotTime(10000);
	portal->SetOverlapType(EActorType::Player);
	portal->SetTeleportLocation(FVector(10050.0f, 10050.0f, 96.0f));

	ActorPtr newActor = this->SpawnActor<Trap<Dungeon, int32, int64>>(this->GetGameObjectRef(), Location(10050.0f, 10050.0f, 100.0f), Rotation(), Scale(1.0f, 1.0f, 1.0f));
	if (nullptr == newActor)
	{
		return;
	}

	std::shared_ptr<Trap<Dungeon, int32, int64>> newTrap = std::static_pointer_cast<Trap<Dungeon, int32, int64>>(newActor);
	if (nullptr == newTrap)
	{
		return;
	}
	newTrap->SetTrigger(true);
	newTrap->SetMaxOverlap(this->mMaxPlayers);
	newTrap->SetOverlapType(EActorType::Player);
	newTrap->SetBoxTriggerExtent(FVector(1000.0f, 1000.0f, 500.0f));
	newTrap->SetEnterTrapCallBackFunction(&Dungeon::PlaySequence, 2, 10000);

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

	mStageCount++;
	return result;
}

bool Dungeon::CheackStageA()
{
	return IsEmptyEnemy();
}

bool Dungeon::CheackStageB()
{
	return IsEmptyEnemy();
}

bool Dungeon::CheackBossStage()
{
	return false;
}

void Dungeon::ClearStageA()
{
	
	this->DestroyActors(static_cast<uint8>(EActorType::Obstruction));
	this->DestroyActors(static_cast<uint8>(EActorType::EnemyAttack));
		
	mEnemySpawnerManger->ClearEnemySpawner();

	ActorPtr newActor = this->SpawnActor<Trap<Dungeon>>(this->GetGameObjectRef(), Location(-6500.0f, -450.0f, 400.0f), Rotation(), Scale(1.0f, 1.0f, 1.0f));
	if (nullptr == newActor)
	{
		return;
	}

	std::shared_ptr<Trap<Dungeon>> newTrap = std::static_pointer_cast<Trap<Dungeon>>(newActor);
	if (nullptr == newTrap)
	{
		return;
	}
	newTrap->SetTrigger(true);
	newTrap->SetMaxOverlap(this->mMaxPlayers);
	newTrap->SetOverlapType(EActorType::Player);
	newTrap->SetBoxTriggerExtent(FVector(2500.0f, 2400.0f, 500.0f));
	newTrap->SetEnterTrapCallBackFunction(&Dungeon::CreateStageB);
}

void Dungeon::ClearStageB()
{

	this->DestroyActors(static_cast<uint8>(EActorType::Obstruction));
	this->DestroyActors(static_cast<uint8>(EActorType::EnemyAttack));

	mEnemySpawnerManger->ClearEnemySpawner();

	std::shared_ptr<Portal> portal = std::static_pointer_cast<Portal>(SpawnActor<Portal>(this->GetGameObjectRef(), Location(-6570.0f, 1005.0f, 474.0), FRotator(), Scale()));
	portal->SetTrigger(true);
	portal->SetMaxOverlap(this->mMaxPlayers);
	portal->SetOverlapType(EActorType::Player);
	portal->SetSphereTriggerRadius(300.0f);
	portal->SetTeleportLocation(FVector(10050.0f, 10050.0f, 96.0f));

	this->CreateBossStage();
}

void Dungeon::ClearBossStage()
{
	this->DestroyActors(static_cast<uint8>(EActorType::Obstruction));
	this->DestroyActors(static_cast<uint8>(EActorType::EnemyAttack));

	mEnemySpawnerManger->ClearEnemySpawner();

	std::shared_ptr<WorldPortal> worldPortal = std::static_pointer_cast<WorldPortal>(SpawnActor<WorldPortal>(this->GetGameObjectRef(), Location(10050.0f, 10050.0f, 0.0f), FRotator(), Scale()));
	worldPortal->SetTrigger(true);
	worldPortal->SetMaxOverlap(this->mMaxPlayers);
	worldPortal->SetSphereTriggerRadius(3000.0f);
	worldPortal->SetWaitWorldTeleprotTime(15000);
	worldPortal->SetOverlapType(EActorType::Player);
	worldPortal->SetTeleportWorld(this->mOriginWorld);
	worldPortal->SetTeleportLocation(FVector(-36900.0f, 38000.0f, 188.0f));
	worldPortal->SetTeleportLevel(std::string("L_Kingdom"));
}

bool Dungeon::IsEmptyEnemy()
{
	std::vector<EnemySpawnerPtr> enemySpawners = mEnemySpawnerManger->GetEnemySpawners();
	if (enemySpawners.size() == 0)
	{
		return false;
	}

	int32 loop = 0;
	int32 least = 0;
	for (auto spawner : enemySpawners)
	{
		loop  += spawner->GetSpawnLoop();
		least += spawner->GetCurrentEnemyCount();
	}

	if (loop == 0 && least == 0)
	{
		return true;
	}

	return false;
}

void Dungeon::PlaySequence(int32 inSequenceID, int64 inSequencePlayTime)
{
	this->mSequenceComponent.SetSequenceID(inSequenceID);
	this->mSequenceComponent.SetPlayTime(inSequencePlayTime);
	this->mSequenceComponent.Play();

	Protocol::S2C_PlaySequence playSequencePacket;
	playSequencePacket.set_sequence(inSequenceID);
	playSequencePacket.set_max_number(this->mMaxPlayers);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, playSequencePacket);
	this->SendWorldPlayers(sendBuffer);
}

void Dungeon::SkipSequence(PlayerStatePtr inPlayerState)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int64& remoteID = remotePlayer->GetGameObjectID();

	if (false == this->mSequenceComponent.Skip(remoteID))
	{
		Protocol::S2C_SkipSequence skipSequence;
		skipSequence.set_max_number(this->mMaxPlayers);
		skipSequence.set_least_number(this->mSequenceComponent.GetSkipPlayers());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, skipSequence);
		this->SendWorldPlayers(sendBuffer);
	}
}

void Dungeon::EndSequence()
{
	int32 sequence = this->mSequenceComponent.GetSequenceID();
	switch (sequence)
	{
	case 1:
		mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(10050.0f, 10050.0f, 200.0f), 0.0f, EnemyID::Enemy_Lich_Phase1, 1, 1, true, true, 1500.0f, 1500.0f);
		break;
	case 2:
		mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(10050.0f, 10050.0f, 200.0f), 0.0f, EnemyID::Enemy_Lich_Phase2, 1, 1, true, true, 1500.0f, 1500.0f);
		break;
	case 3:
		mEnemySpawnerManger->CreateEnemySpawner(this->GetGameObjectRef(), Location(10050.0f, 10050.0f, 200.0f), 0.0f, EnemyID::Enemy_Lich_Phase3, 1, 1, true, true, 1500.0f, 1500.0f);
		break;
	case 4:
		this->ClearBossStage();
		break;
	default:
		break;
	}

	Protocol::S2C_EndSequence endSequencePacket;

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, endSequencePacket);
	this->SendWorldPlayers(sendBuffer);
}

bool Dungeon::IsReady() const
{
	return mState == EDungeonState::State_Ready;
}

bool Dungeon::IsPlay() const
{
	return mState == EDungeonState::State_Play;
}

bool Dungeon::IsDeathPlayers()
{
	if (this->mWorldPlayers.size() == 0)
	{
		InitDungeon();
		return true;
	}

	for (auto players : this->mWorldPlayers)
	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(players.second->GetRemotePlayer());
		if (nullptr == remotePlayer)
		{
			return false;
		}

		PlayerCharacterPtr character = remotePlayer->GetCharacter();
		if (nullptr == character)
		{
			return false;
		}

		if (true == character->IsValid())
		{
			return false;
		}
	}

	return true;
}
