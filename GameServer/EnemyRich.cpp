#include "pch.h"
#include "EnemyRich.h"
#include "Explosion.h"
#include "Blink.h"
#include "SourSpear.h"
#include "SoulShackles.h"
#include "SoulSpark.h"
#include "Rise.h"
#include "EnemyDarkKnight.h"
#include "LichLifeVessel.h"
#include "OnslaughtOfShadows.h"
#include "RealmOfDeath.h"

//==========================//
//       Rich | Rich		//
//==========================//

EnemyRich::EnemyRich(const WCHAR* inName) : EnemyCharacter(inName), mMove(false)
{
	mTempStage = FVector(10050.0f, 10050.0f, 0.0f);
	mTempStageLenght = 800;
}

EnemyRich::~EnemyRich()
{
}

void EnemyRich::OnTick(const int64 inDeltaTime)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	if (false == IsValid())
	{
		return;
	}

	this->mStateManager.UpdateState(inDeltaTime);

	CharacterPtr aggroCharacter = std::static_pointer_cast<Character>(this->GetAggroActor().lock());
	if (aggroCharacter)
	{
		Location currentLocation		= this->GetLocation();
		Location aggroLocation			= aggroCharacter->GetLocation();

		const float enemyHalf			= this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ();
		const float aggroCharacterHalf	= aggroCharacter->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ();
		float diffHalf = enemyHalf - aggroCharacterHalf;
		aggroLocation.SetZ(aggroLocation.GetZ() + diffHalf);

		this->mMovementComponent.Update(this->GetActorPtr(), 0.0f);
		
		if (false == mMove)
		{
			this->mMovementComponent.SetNewDestination(this->GetActorPtr(), currentLocation, aggroLocation, worldTime, 0.0f);
			this->OnMovementEnemy();
		}
		
	}
	else
	{
		this->OnSyncLocation(inDeltaTime);
	}

	if (this->mStatsComponent.IsChanageStats(inDeltaTime))
	{
		this->DetectChangeEnemy();
	}

	//const float debugDuration = 0.1f;
	//PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), this->GetLocation(), 100.0f, debugDuration);

}

void EnemyRich::OnHit(ActorPtr inInstigated, const float inDamage)
{
	if (IsDeath())
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (false == this->GetAggressive())
	{
		this->SetAggroActor(inInstigated);
	}

	const float curArmor = this->mStatsComponent.GetCurrentStats().GetArmor();
	const float era = curArmor / (curArmor + 100.0f);

	const float curHealth = this->mStatsComponent.GetCurrentStats().GetHealth() - inDamage * (1.0f - era);
	this->mStatsComponent.UpdateCurrentStat(EStatType::Stat_Health, curHealth);
	if (curHealth <= 0.0f)
	{
		this->mStateManager.SetState(EStateType::State_Death);
		return;
	}
	this->DetectChangeEnemy();

	Protocol::S2C_HitEnemy hitPacket;
	hitPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, hitPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void EnemyRich::DoMoveLocation(FVector inStartLocation, FVector inEndLocation, int64 inDuration, float inSpeed)
{
	mMove = true;

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	FRotator	direction = (inEndLocation - inStartLocation).Rotator();
	FVector		foward = direction.GetForwardVector();

	this->SetVelocity(inSpeed, inSpeed, 0.0f);

	this->mMovementComponent.SetNewDestination(this->GetActorPtr(), inStartLocation, inEndLocation, worldTime, 0.0f);
	this->OnMovementEnemy();

	Protocol::S2C_AnimationMovementEnemy animationMovementPacket;
	animationMovementPacket.set_object_id(this->GetGameObjectID());
	animationMovementPacket.mutable_start_location()->CopyFrom(PacketUtils::ToSVector(inStartLocation));
	animationMovementPacket.mutable_end_location()->CopyFrom(PacketUtils::ToSVector(inEndLocation));
	animationMovementPacket.set_duration(inDuration);
	animationMovementPacket.set_timestamp(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, animationMovementPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(world->GetNextWorldTime() + inDuration, &EnemyRich::MoveDone);
}

void EnemyRich::DoTeleportLocation(FVector inLocation)
{
	mMove = true;

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	ActorPtr lich = this->GetActorPtr();
	if (nullptr == lich)
	{
		return;
	}

	if (false == IsValid())
	{
		return;
	}

	const Location& originLocation	= inLocation;
	const Rotation& rotation		= lich->GetRotation();
	const FVector& foward			= rotation.GetForwardVector();

	const Location& destination = originLocation + (foward * 100.0f);
	this->mMovementComponent.SetNewDestination(lich, originLocation, destination, worldTime, 0.0f);

	Protocol::S2C_Teleport teleportPacket;
	teleportPacket.set_object_id(this->GetGameObjectID());
	teleportPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(FVector(inLocation)));

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, teleportPacket);
	this->BrodcastPlayerViewers(sendBuffer);

	this->PushTask(world->GetWorldTime() + 100, &EnemyRich::MoveDone);
}

void EnemyRich::MoveDone()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	ActorPtr lich = this->GetActorPtr();
	if (nullptr == lich)
	{
		return;
	}

	this->SetVelocity(0.0f, 0.0f, 0.0f);
	mMove = false;
}

//==========================//
//       Rich | Phase1		//
//==========================//

EnemyRichPhase1::EnemyRichPhase1() : EnemyRich(L"EnemyLichPhase1")
{
}

EnemyRichPhase1::~EnemyRichPhase1()
{
}

void EnemyRichPhase1::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	SetTick(true, GAME_TICK);

	this->SetEnemyType(EnemyType::Boss);

	this->SetEnemeyID(static_cast<int32>(EnemyID::Enemy_Lich_Phase1));
	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);


	this->mStatsComponent.SetSyncTime(GAME_TICK);
	

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(90.0f, 90.0f, 200.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos attackInfos;
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Pattern, attackInfos);

	this->mPatternInfos.push_back(&EnemyRichPhase1::Skill_RiseSkeleton);
	this->mPatternInfos.push_back(&EnemyRichPhase1::Skill_BlinkAttack);
	this->mPatternInfos.push_back(&EnemyRichPhase1::Skill_Explosion);
	this->mPatternInfos.push_back(&EnemyRichPhase1::Skill_MultiCasting);
}

void EnemyRichPhase1::OnPatternShot(ActorPtr inVictim)
{
	int32 pattern = Random::GetIntUniformDistribution(0, static_cast<int32>(mPatternInfos.size() - 1));
	std::function<void(EnemyRichPhase1&)> pattenFunc = mPatternInfos[1];
	pattenFunc(*this);
}

void EnemyRichPhase1::OnPatternOver()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() != EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Search);
		}
	}
}

void EnemyRichPhase1::OnStunWakeUp()
{
	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() == EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Search);
		}
	}
}

void EnemyRichPhase1::OnReward()
{
	DungeonPtr world = std::static_pointer_cast<Dungeon>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldNextTime = world->GetNextWorldTime();
	world->PushTask(worldNextTime + 100, &World::DestroyActors, static_cast<uint8>(EActorType::Enemy));
	world->PushTask(worldNextTime + 100, &World::DestroyActors, static_cast<uint8>(EActorType::EnemyAttack));

	EnemySpawnerManagerPtr spawner = world->GetEnemySpawnerManager();
	if (nullptr == spawner)
	{
		return;
	}
	spawner->PushTask(worldNextTime + 100, &EnemySpawnerManager::ClearEnemySpawner);

	int32 seqID = 2;
	int64 playTime = 5000;
	world->PushTask(worldNextTime + 200, &Dungeon::PlaySequence, seqID, playTime);
}

void EnemyRichPhase1::Skill_RiseSkeleton()
{

	this->GameObjectLog(L"Skill_RiseSkeleton\n");

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	FVector stage = FVector(mTempStage.GetX(), mTempStage.GetY(), mTempStage.GetZ() + 90);
	for (int32 count = 0; count < 5; count++)
	{
		ActorPtr actor = world->SpawnActor<Rise>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(stage, 1000.0f), FRotator(), Scale(1.0f, 1.0f, 1.0f));
		std::shared_ptr<Rise> newRise = std::static_pointer_cast<Rise>(actor);
		if (nullptr == newRise)
		{
			return;
		}
		newRise->SetSpawnInfo(EnemyID::Enemy_Warrior_Skeleton, 1, 1, false, 0.0f, 1500.0f, static_cast<int32>(ESkillID::Skill_Rich_Rise_Skeleton));
		newRise->PushTask(worldTime + 3000, &Rise::SpawnEnemy);
	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Rise));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(worldTime + 10000, &EnemyRichPhase1::OnPatternOver);
}

void EnemyRichPhase1::Skill_BlinkAttack()
{

	this->GameObjectLog(L"Skill_BlinkAttack\n");

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	std::vector<ActorPtr> targetActors;
	bool result = world->FindActors(this->GetLocation(), 2000.0f, static_cast<uint8>(EActorType::Player), targetActors, 1);
	if (false == result)
	{
		return;
	}

	PlayerCharacterPtr player = std::static_pointer_cast<PlayerCharacter>(targetActors[0]);
	if (nullptr == player)
	{
		return;
	}

	const float enemyHalf			= this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ();
	const float aggroCharacterHalf	= player->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ();
	float diffHalf					= enemyHalf - aggroCharacterHalf;

	const Location& playerLocation	= player->GetLocation();
	const Rotation& playerRotation	= player->GetRotation();
	const FVector&	foward			= playerRotation.GetForwardVector();

	const Location spawnLocation	= this->GetLocation();
	const Location blinkLocation	= FVector(playerLocation.GetX(), playerLocation.GetY(), playerLocation.GetZ() + diffHalf) - (foward * 80.0f);
	const Location safeLocation		= Random::GetRandomVectorInRange2D(FVector(mTempStage.GetX(), mTempStage.GetY(), enemyHalf), mTempStageLenght);

	ActorPtr actor = world->SpawnActor<BlinkAttack>(this->GetGameObjectRef(), blinkLocation, this->GetRotation(), Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<BlinkAttack> newBlinkAttack = std::static_pointer_cast<BlinkAttack>(actor);
	if (nullptr == newBlinkAttack)
	{
		return;
	}
	newBlinkAttack->SetBlinkID(ESkillID::Skill_Rich_Blink_Attack);
	newBlinkAttack->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Melee);
	newBlinkAttack->SetTargetActorType(EActorType::Player);
	newBlinkAttack->SetDamage(this->GetEnemyStatsComponent().GetCurrentStats().GetAttackDamage());

	static int64 blinkTime		= 2000;
	static int64 parryingStart	= 480;
	static int64 parryingEnd	= 880;
	static int64 attackEnd		= 1830;

	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), spawnLocation, 90.0f, debugDuration);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), blinkLocation, 90.0f, debugDuration);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), safeLocation, 90.0f, debugDuration);

	//패링 타이밍
	newBlinkAttack->SetParryinglTime(worldTime + blinkTime + parryingStart, worldTime + blinkTime + parryingStart + parryingEnd);

	//공격전 이동
	newBlinkAttack->PushTask(worldTime + blinkTime, &BlinkAttack::TeleportPlayerLocation, blinkLocation, playerRotation);

	//리액션 (연기 나옴)
	newBlinkAttack->PushTask(worldTime + blinkTime, &BlinkAttack::ReActionSkill, blinkLocation, playerRotation);

	//공격
	newBlinkAttack->PushTask(worldTime + blinkTime + parryingEnd, &BlinkAttack::CheackCollision);

	//공격후 이동
	newBlinkAttack->PushTask(worldTime + blinkTime + attackEnd, &BlinkAttack::TeleportSafeLocation, safeLocation, FRotator::TurnRotator(playerRotation));

	//삭제
	newBlinkAttack->PushReserveDestroy(worldTime + 10000);
}

void EnemyRichPhase1::Skill_Explosion()
{

	this->GameObjectLog(L"Skill_Explosion\n");

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	FVector stage = mTempStage;

	for (int32 count = 0; count < 5; ++count)
	{
		const Location		newLocation	= Random::GetRandomVectorInRange2D(stage, mTempStageLenght);
		const Rotation		newRoation	= FRotator();
		const Scale			newScale	= Scale(1.0f, 1.0f, 1.0f);

		ActorPtr actor = world->SpawnActor<Explosion>(this->GetGameObjectRef(), newLocation, newRoation, newScale);
		std::shared_ptr<Explosion> newExplosion = std::static_pointer_cast<Explosion>(actor);
		if (nullptr == newExplosion)
		{
			return;
		}

	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Rise));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(worldTime + 10000, &EnemyRichPhase1::OnPatternOver);
}

void EnemyRichPhase1::Skill_MultiCasting()
{

	this->GameObjectLog(L"Skill_MultiCasting\n");

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr aggroActor = this->GetAggroActor().lock();
	if (nullptr == aggroActor)
	{
		std::vector<ActorPtr> targetActors;
		bool result = world->FindActors(this->GetLocation(), 2000.0f, static_cast<uint8>(EActorType::Player), targetActors, 1);
		if (false == result)
		{
			return;
		}

		aggroActor = targetActors.at(0);
	}
	
	PlayerCharacterPtr player = std::static_pointer_cast<PlayerCharacter>(aggroActor);
	if (nullptr == player)
	{
		return;
	}

	const Location& richLocation	= this->GetLocation();
	const Rotation& richRotation	= this->GetRotation();
	const FVector&	right			= richRotation.GetRightVector();


	for (int32 count = 0; count < 5; ++count)
	{

		if (nullptr == player)
		{
			return;
		}

		float fCount = static_cast<float>(count);
		Location sourSpearLocation = richLocation + (FVector((fCount * 75.0f) - 150.0f, (fCount * 75.0f) - 150.0f, 0.0f) * right);
		float sourSpearZ = sourSpearLocation.GetZ();

		switch (count)
		{
		case 0:
			sourSpearLocation.SetZ(sourSpearZ + 150.0f);
			break;
		case 1:
			sourSpearLocation.SetZ(sourSpearZ + 200.0f);
			break;
		case 2:
			sourSpearLocation.SetZ(sourSpearZ + 250.0f);
			break;
		case 3:
			sourSpearLocation.SetZ(sourSpearZ + 200.0f);
			break;
		case 4:
			sourSpearLocation.SetZ(sourSpearZ + 150.0f);
			break;
		default:
			break;
		}
		
		this->PushTask(worldTime + (count * 700), &EnemyRichPhase1::Skill_SourSpear, aggroActor, sourSpearLocation);

	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Multi_Castring));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(worldTime + 10000, &EnemyRichPhase1::OnPatternOver);
}

void EnemyRichPhase1::Skill_SourSpear(ActorPtr inPlayer, Location inLocation)
{

	this->GameObjectLog(L"Skill_SourSpear\n");
	
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (nullptr == inPlayer)
	{
		return;
	}

	const Location& location = inPlayer->GetLocation();
	const Rotation& rotation = (location - inLocation).Rotator();

	ActorPtr actor = world->SpawnActor<SourSpear>(this->GetGameObjectRef(), inLocation, rotation, Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<SourSpear> newSourSpear = std::static_pointer_cast<SourSpear>(actor);
	if (nullptr == newSourSpear)
	{
		return;
	}
	newSourSpear->SetTargetActorType(EActorType::Player);
}

//==========================//
//       Rich | Phase2		//
//==========================//

EnemyRichPhase2::EnemyRichPhase2() : EnemyRich(L"EnemyLichPhase2"), mSpawnDarkKnight(false)
{
}

EnemyRichPhase2::~EnemyRichPhase2()
{
}

void EnemyRichPhase2::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	SetTick(true, GAME_TICK);

	this->SetEnemyType(EnemyType::Boss);

	this->SetEnemeyID(static_cast<int32>(EnemyID::Enemy_Lich_Phase2));
	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);


	this->mStatsComponent.SetSyncTime(GAME_TICK);
	

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(90.0f, 90.0f, 200.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos attackInfos;
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Pattern, attackInfos);

	this->mPatternInfos.push_back(&EnemyRichPhase2::Skill_BlinkSturn);
	this->mPatternInfos.push_back(&EnemyRichPhase2::Skill_SoulSpark);
	this->mPatternInfos.push_back(&EnemyRichPhase2::Skill_SoulShackles);
}

void EnemyRichPhase2::OnPatternShot(ActorPtr inVictim)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& nextWorldTime = world->GetNextWorldTime();

	if (false == mSpawnDarkKnight)
	{
		mSpawnDarkKnight = true;
		this->PushTask(nextWorldTime, &EnemyRichPhase2::Skill_RiseDarkKnight);
		this->PushTask(nextWorldTime + 5000, &EnemyRichPhase2::OnPatternOver);
	}
	else
	{
		//int32 pattern = Random::GetIntUniformDistribution(0, static_cast<int32>(mPatternInfos.size() - 1));
		//std::function<void(EnemyRichPhase2&)> pattenFunc = mPatternInfos[2];
		//pattenFunc(*this);
	}
}

void EnemyRichPhase2::OnPatternOver()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() != EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Search);
		}
	}
}

void EnemyRichPhase2::OnStunWakeUp()
{
	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() == EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Search);
		}
	}
}

void EnemyRichPhase2::OnReward()
{
	DungeonPtr world = std::static_pointer_cast<Dungeon>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldNextTime = world->GetNextWorldTime();
	world->PushTask(worldNextTime + 100, &World::DestroyActors, static_cast<uint8>(EActorType::Enemy));
	world->PushTask(worldNextTime + 100, &World::DestroyActors, static_cast<uint8>(EActorType::EnemyAttack));

	EnemySpawnerManagerPtr spawner = world->GetEnemySpawnerManager();
	if (nullptr == spawner)
	{
		return;
	}
	spawner->PushTask(worldNextTime + 100, &EnemySpawnerManager::ClearEnemySpawner);

	int32 seqID = 3;
	int64 playTime = 3000;
	world->PushTask(worldNextTime + 200, &Dungeon::PlaySequence, seqID, playTime);
}

void EnemyRichPhase2::Skill_RiseDarkKnight()
{

	this->GameObjectLog(L"Skill_RiseDarkKnight\n");

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	FVector stage = FVector(mTempStage.GetX(), mTempStage.GetY(), mTempStage.GetZ() + 173);

	ActorPtr actor = world->SpawnActor<Rise>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(stage, 300.0f), FRotator(), Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<Rise> newRise = std::static_pointer_cast<Rise>(actor);
	if (nullptr == newRise)
	{
		return;
	}
	newRise->SetSpawnInfo(EnemyID::Enemy_Dark_Knight, 1, 1, true, 0.0f, 1500.0f, static_cast<int32>(ESkillID::Skill_Rich_Rise_DarkKnight));
	newRise->PushTask(worldTime + 3000, &Rise::SpawnEnemy);
}

void EnemyRichPhase2::Skill_BlinkSturn()
{

	this->GameObjectLog(L"Skill_BlinkSturn\n");

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	FVector stage = mTempStage;
	stage.SetZ(200.0f);
	const Location newLocation = Random::GetRandomVectorInRange2D(stage, mTempStageLenght);

	std::vector<ActorPtr> targetActors;
	bool result = world->FindActors(this->GetLocation(), 2000.0f, static_cast<uint8>(EActorType::Player), targetActors, 1);
	if (false == result)
	{
		return;
	}

	PlayerCharacterPtr player = std::static_pointer_cast<PlayerCharacter>(targetActors[0]);
	if (nullptr == player)
	{
		return;
	}

	const float enemyHalf = this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ();
	const float aggroCharacterHalf = player->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetZ();
	float diffHalf = enemyHalf - aggroCharacterHalf;

	const Location& playerLocation = player->GetLocation();
	const Rotation& playerRotation = player->GetRotation();
	const FVector& foward = playerRotation.GetForwardVector();

	const Location spawnLocation = this->GetLocation();
	const Location blinkLocation = FVector(playerLocation.GetX(), playerLocation.GetY(), playerLocation.GetZ() + diffHalf) - (foward * 80.0f);
	const Location safeLocation = Random::GetRandomVectorInRange2D(FVector(mTempStage.GetX(), mTempStage.GetY(), enemyHalf), mTempStageLenght);

	ActorPtr actor = world->SpawnActor<BlinkSturn>(this->GetGameObjectRef(), spawnLocation, playerRotation, Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<BlinkSturn> newBlinkSturn = std::static_pointer_cast<BlinkSturn>(actor);
	if (nullptr == newBlinkSturn)
	{
		return;
	}
	newBlinkSturn->SetBlinkID(ESkillID::Skill_Rich_Blink_Sturn);
	newBlinkSturn->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Hard_Melee);
	newBlinkSturn->SetTargetActorType(EActorType::Player);
	newBlinkSturn->SetDamage(150.0f);

	static int64 blinkTime = 2000;
	static int64 parryingStart = 430;
	static int64 parryingEnd = 530;
	static int64 attackEnd = 1670;
	static int64 sturnEnd = 2000;

	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), spawnLocation, 90.0f, debugDuration);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), blinkLocation, 90.0f, debugDuration);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), safeLocation, 90.0f, debugDuration);

	//패링 타이밍
	newBlinkSturn->SetParryinglTime(worldTime + blinkTime + parryingStart, worldTime + blinkTime + parryingStart + parryingEnd);

	//공격전 이동
	newBlinkSturn->PushTask(worldTime + blinkTime, &BlinkSturn::TeleportPlayerLocation, blinkLocation, playerRotation);

	//리액션
	newBlinkSturn->PushTask(worldTime + blinkTime, &BlinkSturn::ReActionSkill, blinkLocation, playerRotation);

	//공격
	newBlinkSturn->PushTask(worldTime + blinkTime + parryingEnd, &BlinkSturn::CheackCollision);

	//공격후 이동
	newBlinkSturn->PushTask(worldTime + blinkTime + attackEnd, &BlinkSturn::TeleportSafeLocation, safeLocation, FRotator::TurnRotator(playerRotation));

	world->PushTask(world->GetNextWorldTime() + blinkTime + attackEnd + sturnEnd, &GameWorld::DestroyActor, newBlinkSturn->GetGameObjectID());

	this->PushTask(worldTime + 10000, &EnemyRichPhase2::OnPatternOver);
}

void EnemyRichPhase2::Skill_SoulSpark()
{

	this->GameObjectLog(L"Skill_SoulSpark\n");

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr aggroActor = this->GetAggroActor().lock();
	if (nullptr == aggroActor)
	{
		return;
	}

	FVector		location	= this->GetLocation();
	FRotator	rotation	= this->GetRotation();
	FVector		foward		= rotation.GetForwardVector();
	const float collision	= this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();

	ActorPtr actor = world->SpawnActor<SoulSpark>(this->GetGameObjectRef(), location + (foward * collision), rotation, Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<SoulSpark> newSoulSpark = std::static_pointer_cast<SoulSpark>(actor);
	if (nullptr == newSoulSpark)
	{
		return;
	}

	this->PushTask(worldTime + 10000, &EnemyRichPhase2::OnPatternOver);
}

void EnemyRichPhase2::Skill_SoulShackles()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr aggroActor = this->GetAggroActor().lock();
	if (nullptr == aggroActor)
	{
		return;
	}

	Location location = aggroActor->GetLocation();
	Rotation rotation = aggroActor->GetRotation();

	ActorPtr actor = world->SpawnActor<SoulShackles>(this->GetGameObjectRef(), location, rotation, Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<SoulShackles> newSoulShackles = std::static_pointer_cast<SoulShackles>(actor);
	if (nullptr == newSoulShackles)
	{
		return;
	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Rise));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(worldTime + 10000, &EnemyRichPhase2::OnPatternOver);
}

//==========================//
//       Rich | Phase3		//
//==========================//

EnemyRichPhase3::EnemyRichPhase3() : EnemyRich(L"EnemyLichPhase3"), mSpawnLifeVessel(false)
{
}

EnemyRichPhase3::~EnemyRichPhase3()
{
}

void EnemyRichPhase3::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, GAME_TICK);

	this->SetEnemyType(EnemyType::Boss);

	this->SetEnemeyID(static_cast<int32>(EnemyID::Enemy_Lich_Phase3));
	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(90.0f, 90.0f, 200.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());


	AttackInfos attackInfos;
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Pattern, attackInfos);

	this->mPatternInfos.push_back(&EnemyRichPhase3::Skill_RiseDarkSkeleton);
	this->mPatternInfos.push_back(&EnemyRichPhase3::Skill_OnslaughtOfShadows);
}

void EnemyRichPhase3::OnPatternShot(ActorPtr inVictim)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& nextWorldTime = world->GetNextWorldTime();

	if (false == mSpawnLifeVessel)
	{
		mSpawnLifeVessel = true;
		this->PushTask(nextWorldTime, &EnemyRichPhase3::Skill_LifeVessel);
		this->PushTask(nextWorldTime, &EnemyRichPhase3::Skill_RealmOfDeath);
		this->PushTask(nextWorldTime + 5000, &EnemyRichPhase3::OnPatternOver);
	}
	else
	{
		int32 pattern = Random::GetIntUniformDistribution(0, static_cast<int32>(mPatternInfos.size() - 1));
		std::function<void(EnemyRichPhase3&)> pattenFunc = mPatternInfos[0];
		pattenFunc(*this);
	}
}

void EnemyRichPhase3::OnPatternOver()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() != EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Search);
		}
	}
}

void EnemyRichPhase3::OnStunWakeUp()
{
	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() == EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Search);
		}
	}
}

void EnemyRichPhase3::OnReward()
{
	DungeonPtr world = std::static_pointer_cast<Dungeon>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldNextTime = world->GetNextWorldTime();
	world->PushTask(worldNextTime + 100, &World::DestroyActors, static_cast<uint8>(EActorType::Enemy));
	world->PushTask(worldNextTime + 100, &World::DestroyActors, static_cast<uint8>(EActorType::EnemyAttack));

	EnemySpawnerManagerPtr spawner = world->GetEnemySpawnerManager();
	if (nullptr == spawner)
	{
		return;
	}
	spawner->PushTask(worldNextTime + 100, &EnemySpawnerManager::ClearEnemySpawner);

	int32 seqID = 4;
	int64 playTime = 5000;
	world->PushTask(worldNextTime + 200, &Dungeon::PlaySequence, seqID, playTime);
}

void EnemyRichPhase3::Skill_RiseDarkSkeleton()
{

	this->GameObjectLog(L"Skill_RiseDarkSkeleton\n");

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	FVector stage = FVector(mTempStage.GetX(), mTempStage.GetY(), mTempStage.GetZ() + 90);
	for (int32 count = 0; count < 5; count++)
	{
		ActorPtr actor = world->SpawnActor<Rise>(this->GetGameObjectRef(), Random::GetRandomVectorInRange2D(stage, 1000.0f), FRotator(), Scale(1.0f, 1.0f, 1.0f));
		std::shared_ptr<Rise> newRise = std::static_pointer_cast<Rise>(actor);
		if (nullptr == newRise)
		{
			return;
		}
		newRise->SetSpawnInfo(EnemyID::Enemy_Dark_Skeleton, 3, 1, false, 0.0f, 1500.0f, static_cast<int32>(ESkillID::Skill_Rich_Rise_DarkSkeleton));
		newRise->PushTask(worldTime + 3000, &Rise::SpawnEnemy);
	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Rise));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(worldTime + 10000, &EnemyRichPhase3::OnPatternOver);
}

void EnemyRichPhase3::Skill_RealmOfDeath()
{

	this->GameObjectLog(L"Skill_RealmOfDeath\n");

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetNextWorldTime();

	FVector stage = mTempStage;

	ActorPtr actor = world->SpawnActor<RealmOfDeath>(this->GetGameObjectRef(), stage, FRotator(), Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<RealmOfDeath> newRealOfDeath = std::static_pointer_cast<RealmOfDeath>(actor);
	if (nullptr == newRealOfDeath)
	{
		return;
	}
}

void EnemyRichPhase3::Skill_OnslaughtOfShadows()
{

	this->GameObjectLog(L"Skill_OnslaughtOfShadows\n");

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetNextWorldTime();

	FVector stage = mTempStage;
	stage.SetZ(200.0f);

	const Location	randomLocation	= Random::GetRandomVectorInRange2D(stage, mTempStageLenght);
	FRotator		randomRotation	= FRotator(0.0f, static_cast<float>(Random::GetRealUniformDistribution(-180, 180)), 0.0f);

	ActorPtr actor = world->SpawnActor<OnslaughtOfShadows>(this->GetGameObjectRef(), randomLocation, randomRotation, Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<OnslaughtOfShadows> newOnslaughtOfShadows = std::static_pointer_cast<OnslaughtOfShadows>(actor);
	if (nullptr == newOnslaughtOfShadows)
	{
		return;
	}
	FVector LU = FVector(11100.0f	, 9000.0f	, 200.0f);
	FVector RU = FVector(11100.0f	, 11100.0f	, 200.0f);
	FVector RD = FVector(9000.0f	, 11100.0f	, 200.0f);
	FVector LD = FVector(9000.0f	, 9000.0f	, 200.0f);
	newOnslaughtOfShadows->PushTask(world->GetNextWorldTime(), &OnslaughtOfShadows::SetOnslaughtOfShadows, LU, RU, RD, LD, stage);

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Rise));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(worldTime + 10000, &EnemyRichPhase3::OnPatternOver);
}

void EnemyRichPhase3::Skill_LifeVessel()
{

	this->GameObjectLog(L"Skill_LifeVessel\n");

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& nextWorldTime = world->GetNextWorldTime();
	FVector stage = mTempStage;
	stage.SetZ(48.0f);

	ActorPtr actor = world->SpawnActor<Rise>(this->GetGameObjectRef(), stage, FRotator(), Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<Rise> newRise = std::static_pointer_cast<Rise>(actor);
	if (nullptr == newRise)
	{
		return;
	}
	newRise->SetSpawnInfo(EnemyID::Enemy_Lich_Life_Vessle, 1, 1, true, 1000.0f, 1500.0f, static_cast<int32>(ESkillID::Skill_Rich_Life_Vessel));
	newRise->PushTask(nextWorldTime, &Rise::SpawnEnemy);
	

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Rise));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(world->GetWorldTime());

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}
