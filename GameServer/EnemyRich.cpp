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

//==========================//
//       Rich | Rich		//
//==========================//

EnemyRich::EnemyRich(const WCHAR* inName) : EnemyCharacter(inName)
{
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

	this->OnSyncLocation(inDeltaTime);

	this->mStateManager.UpdateState(inDeltaTime);

	if (this->mStatsComponent.IsChanageStats(inDeltaTime))
	{
		this->DetectChangeEnemy();
	}

}

//==========================//
//       Rich | Phase1		//
//==========================//

EnemyRichPhase1::EnemyRichPhase1() : EnemyRich(L"EnemyRichPhase1")
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

	SetTick(true, SYSTEM_TICK);

	this->SetEnemeyID(5);
	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (datas)
	{
		this->mStatsComponent.SetSyncTime(GAME_TICK);
		this->mStatsComponent.InitMaxStats(datas->GetEnemyStat(5));
	}

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

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
	std::function<void(EnemyRichPhase1&)> pattenFunc = mPatternInfos[pattern];
	pattenFunc(*this);
}

void EnemyRichPhase1::OnPatternOver()
{
	if (false == this->IsDeath())
	{
		this->mStateManager.SetState(EStateType::State_Search);
	}
}

void EnemyRichPhase1::OnReward()
{
}

void EnemyRichPhase1::Skill_RiseSkeleton()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	FVector stage = FVector(10000.0f, 10000.0f, 100.0f);

	for (int32 count = 0; count < 5; ++count)
	{

		const Location		newLocation = Random::GetRandomVectorInRange2D(stage, 800);
		const Rotation		newRoation = (stage - newLocation).Rotator();

		ActorPtr actor = world->SpawnActor<Rise>(this->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f));
		std::shared_ptr<Rise> newRise = std::static_pointer_cast<Rise>(actor);
		if (nullptr == newRise)
		{
			return;
		}
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
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	FVector stage = FVector(10000.0f, 10000.0f, 100.0f);
	const Location newLocation = Random::GetRandomVectorInRange2D(stage, 800);

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

	const Location& playerLocation			= player->GetLocation();
	const Rotation& playerRotation			= player->GetRotation();
	const FVector&	foward					= playerRotation.GetForwardVector();
	const float&	radius					= player->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();
	const Location& playerBehindkLocation	= playerLocation - (foward * radius);
	const Scale		scale = Scale(1.0f, 1.0f, 1.0f);

	ActorPtr actor = world->SpawnActor<BlinkAttack>(this->GetGameObjectRef(), playerBehindkLocation, playerRotation, scale);
	std::shared_ptr<BlinkAttack> newBlinkAttack = std::static_pointer_cast<BlinkAttack>(actor);
	if (nullptr == newBlinkAttack)
	{
		return;
	}
	newBlinkAttack->SetBlinkID(ESkillID::Skill_Rich_Blink_Attack);
	newBlinkAttack->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Melee);
	newBlinkAttack->SetTargetActorType(EActorType::Player);
	newBlinkAttack->SetDamage(150.0f);

	static int64 blinkTime		= 2000;
	static int64 parryingStart	= 300;
	static int64 parryingEnd	= 200;
	static int64 attackEnd		= 1000;

	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), playerBehindkLocation, radius, debugDuration);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), newLocation, radius, debugDuration);

	//패링 타이밍
	newBlinkAttack->SetParryinglTime(worldTime + blinkTime + parryingStart, worldTime + blinkTime + parryingStart + parryingEnd);

	//공격전 이동
	newBlinkAttack->PushTask(worldTime + blinkTime, &BlinkAttack::TeleportPlayerLocation, playerBehindkLocation, playerRotation);

	//공격
	newBlinkAttack->PushTask(worldTime + blinkTime + parryingStart + parryingEnd, &BlinkAttack::CheackCollision);

	//공격후 이동
	newBlinkAttack->PushTask(worldTime + blinkTime + attackEnd, &BlinkAttack::TeleportSafeLocation, newLocation, FRotator::TurnRotator(playerRotation));

	this->PushTask(worldTime + 10000, &EnemyRichPhase1::OnPatternOver);
}

void EnemyRichPhase1::Skill_Explosion()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	FVector stage = FVector(10000.0f, 10000.0f, 100.0f);

	for (int32 count = 0; count < 5; ++count)
	{
		const Location		newLocation	= Random::GetRandomVectorInRange2D(stage, 800);
		const Rotation		newRoation	= FRotator();
		const Scale			newScale	= Scale(1.0f, 1.0f, 1.0f);

		ActorPtr actor = world->SpawnActor<Explosion>(this->GetGameObjectRef(), newLocation, newRoation, newScale);
		std::shared_ptr<Explosion> newExplosion = std::static_pointer_cast<Explosion>(actor);
		if (nullptr == newExplosion)
		{
			return;
		}

	}

	this->PushTask(worldTime + 10000, &EnemyRichPhase1::OnPatternOver);
}

void EnemyRichPhase1::Skill_MultiCasting()
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
		case 1:
			sourSpearLocation.SetZ(sourSpearZ + 100.0f);
			break;
		case 2:
			sourSpearLocation.SetZ(sourSpearZ + 150.0f);
			break;
		case 3:
			sourSpearLocation.SetZ(sourSpearZ + 200.0f);
			break;
		case 4:
			sourSpearLocation.SetZ(sourSpearZ + 150.0f);
			break;
		case 5:
			sourSpearLocation.SetZ(sourSpearZ + 100.0f);
			break;
		default:
			break;
		}
		
		this->PushTask(worldTime + (count * 1000), &EnemyRichPhase1::Skill_SourSpear, aggroActor, sourSpearLocation);

	}

	this->PushTask(worldTime + 10000, &EnemyRichPhase1::OnPatternOver);
}

void EnemyRichPhase1::Skill_SourSpear(ActorPtr inPlayer, Location inLocation)
{
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

EnemyRichPhase2::EnemyRichPhase2() : EnemyRich(L"EnemyRichPhase2")
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

	SetTick(true, SYSTEM_TICK);

	this->SetEnemeyID(6);
	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (datas)
	{
		this->mStatsComponent.SetSyncTime(GAME_TICK);
		this->mStatsComponent.InitMaxStats(datas->GetEnemyStat(6));
	}

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos attackInfos;
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Pattern, attackInfos);

	this->mPatternInfos.push_back(&EnemyRichPhase2::Skill_BlinkSturn);
	this->mPatternInfos.push_back(&EnemyRichPhase2::Skill_SoulSpark);
	this->mPatternInfos.push_back(&EnemyRichPhase2::Skill_SoulShackles);
}

void EnemyRichPhase2::OnPatternShot(ActorPtr inVictim)
{
	int32 pattern = Random::GetIntUniformDistribution(0, static_cast<int32>(mPatternInfos.size() - 1));
	std::function<void(EnemyRichPhase2&)> pattenFunc = mPatternInfos[pattern];
	pattenFunc(*this);
}

void EnemyRichPhase2::OnPatternOver()
{
	if (false == this->IsDeath())
	{
		this->mStateManager.SetState(EStateType::State_Chase);
	}
}

void EnemyRichPhase2::OnReward()
{
}

void EnemyRichPhase2::Skill_RiseDarkKnight()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	FVector		location = this->GetLocation();
	FRotator	rotation = this->GetRotation();
	FVector		foward = rotation.GetForwardVector() * 200.0f;

	ActorPtr actor = world->SpawnActor<EnemyDarkKnight>(this->GetGameObjectRef(), location + foward, rotation, Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<EnemyDarkKnight> darkKnight = std::static_pointer_cast<EnemyDarkKnight>(actor);
	if (nullptr == darkKnight)
	{
		return;
	}
}

void EnemyRichPhase2::Skill_BlinkSturn()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	FVector stage = FVector(10000.0f, 10000.0f, 100.0f);
	const Location newLocation = Random::GetRandomVectorInRange2D(stage, 800);

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

	const Location& playerLocation = player->GetLocation();
	const Rotation& playerRotation = player->GetRotation();
	const FVector& foward = playerRotation.GetForwardVector();
	const float& radius = player->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();
	const Location& playerBehindkLocation = playerLocation - (foward * radius);
	const Scale		scale = Scale(1.0f, 1.0f, 1.0f);

	ActorPtr actor = world->SpawnActor<BlinkSturn>(this->GetGameObjectRef(), playerBehindkLocation, playerRotation, scale);
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
	static int64 parryingStart = 300;
	static int64 parryingEnd = 200;
	static int64 attackEnd = 1000;

	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), playerBehindkLocation, radius, debugDuration);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), newLocation, radius, debugDuration);

	//패링 타이밍
	//newBlinkSturn->SetParryinglTime(worldTime + blinkTime + parryingStart, worldTime + blinkTime + parryingStart + parryingEnd);

	//공격전 이동
	newBlinkSturn->PushTask(worldTime + blinkTime, &BlinkSturn::TeleportPlayerLocation, playerBehindkLocation, playerRotation);

	//공격
	newBlinkSturn->PushTask(worldTime + blinkTime + parryingStart + parryingEnd, &BlinkSturn::CheackCollision);

	//공격후 이동
	newBlinkSturn->PushTask(worldTime + blinkTime + attackEnd, &BlinkSturn::TeleportSafeLocation, newLocation, FRotator::TurnRotator(playerRotation));

	this->PushTask(worldTime + 10000, &EnemyRichPhase2::OnPatternOver);
}

void EnemyRichPhase2::Skill_SoulSpark()
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

	world->PushTask(worldTime + 5000, &GameWorld::DestroyActor, newSoulShackles->GetGameObjectID());

	this->PushTask(worldTime + 10000, &EnemyRichPhase2::OnPatternOver);
}

//==========================//
//       Rich | Phase3		//
//==========================//

EnemyRichPhase3::EnemyRichPhase3() : EnemyRich(L"EnemyRichPhase3")
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

	SetTick(true, SYSTEM_TICK);

	this->SetEnemeyID(7);
	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (datas)
	{
		this->mStatsComponent.SetSyncTime(GAME_TICK);
		this->mStatsComponent.InitMaxStats(datas->GetEnemyStat(7));
	}

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());


	AttackInfos attackInfos;
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Pattern, attackInfos);

	this->mPatternInfos.push_back(&EnemyRichPhase3::RiseSkeleton);
	this->mPatternInfos.push_back(&EnemyRichPhase3::OnslaughtOfShadows);

	this->RealmOfDeath();
	this->LifeVessel();
}

void EnemyRichPhase3::OnPatternShot(ActorPtr inVictim)
{
	int32 pattern = Random::GetIntUniformDistribution(0, static_cast<int32>(mPatternInfos.size() - 1));
	std::function<void(EnemyRichPhase3&)> pattenFunc = mPatternInfos[pattern];
	pattenFunc(*this);
}

void EnemyRichPhase3::OnPatternOver()
{
	if (false == this->IsDeath())
	{
		this->mStateManager.SetState(EStateType::State_Search);
	}
}

void EnemyRichPhase3::OnReward()
{
}

void EnemyRichPhase3::RiseSkeleton()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	FVector stage = FVector(10000.0f, 10000.0f, 100.0f);

	for (int32 count = 0; count < 5; ++count)
	{

		const Location		newLocation = Random::GetRandomVectorInRange2D(stage, 800);
		const Rotation		newRoation = (stage - newLocation).Rotator();

		ActorPtr actor = world->SpawnActor<Rise>(this->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f));
		std::shared_ptr<Rise> newRise = std::static_pointer_cast<Rise>(actor);
		if (nullptr == newRise)
		{
			return;
		}
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

void EnemyRichPhase3::RealmOfDeath()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetNextWorldTime();
	FVector stage = FVector(10000.0f, 10000.0f, 0.0f);

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Realm_Of_Death));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(stage));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(FRotator()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}

void EnemyRichPhase3::OnslaughtOfShadows()
{
}

void EnemyRichPhase3::LifeVessel()
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetNextWorldTime();
	FVector stage = FVector(10000.0f, 10000.0f, 100.0f);

	const Location newLocation = Random::GetRandomVectorInRange2D(stage, 800);

	ActorPtr actor = world->SpawnActor<LichLifeVessel>(this->GetGameObjectRef(), newLocation, FRotator(), Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<LichLifeVessel> newLifeVessel = std::static_pointer_cast<LichLifeVessel>(actor);
	if (nullptr == newLifeVessel)
	{
		return;
	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Life_Vessel));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(newLocation));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(FRotator()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}
