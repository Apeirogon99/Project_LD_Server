#include "pch.h"
#include "EnemyDarkKnight.h"
#include "Extermination.h"

DarkKnightAttackInfo::DarkKnightAttackInfo() : mExtent(), mMulDamage(0.0f), mParryingTime(0), mCheckCollisionTime(0)
{
}

DarkKnightAttackInfo::DarkKnightAttackInfo(const EDarkKnightAttackType& inSkillID, const FVector& inExtent, const float& inDamage, const int64& inParryingTime, const int64& inCheckCollisionTime)
{
	mSkillID = inSkillID;
	mExtent = inExtent;
	mMulDamage = inDamage;
	mParryingTime = inParryingTime;
	mCheckCollisionTime = inCheckCollisionTime;
}

DarkKnightAttackInfo::~DarkKnightAttackInfo()
{
}


EnemyDarkKnight::EnemyDarkKnight() : EnemyCharacter(L"DarkKnight"), mIsBerserkMode(false)
{
}

EnemyDarkKnight::~EnemyDarkKnight()
{
}

void EnemyDarkKnight::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);
	this->mSpawnParticle = static_cast<int32>(ESkillID::Skill_Rich_Rise_DarkKnight);
	this->mSpawnParticleDelay = 3000;

	this->SetEnemyType(EnemyType::Boss);

	this->SetEnemeyID(static_cast<int32>(EnemyID::Enemy_Dark_Knight));
	this->SetAggressive(true);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(63.0f, 63.0f, 173.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	AttackInfos attackInfos;
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Pattern, attackInfos);

	this->mPatternInfos.push_back(&EnemyDarkKnight::RunningAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::ChargedComboAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::UppercutAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::SwingAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::SwingAndSlamAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::HandAndSwordSwipeAttack);

	//																														범위	 - 대미지 증가량 - 패링 - 타겟 타이밍
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::Running,			DarkKnightAttackInfo(EDarkKnightAttackType::Running,			FVector(400.0f, 100.0f, 100.0f), 3.0f, 300, 500)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::UpperCut,			DarkKnightAttackInfo(EDarkKnightAttackType::UpperCut,			FVector(140.0f, 100.0f, 100.0f), 1.8f, 0, 200)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::LeftToRightSwing, DarkKnightAttackInfo(EDarkKnightAttackType::LeftToRightSwing, FVector(140.0f, 180.0f, 100.0f), 1.5f, 0, 200)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::RightToLeftSwing, DarkKnightAttackInfo(EDarkKnightAttackType::RightToLeftSwing, FVector(140.0f, 180.0f, 100.0f), 1.5f, 0, 200)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::Slam,				DarkKnightAttackInfo(EDarkKnightAttackType::Slam,				FVector(160.0f, 100.0f, 100.0f), 2.0f, 250, 300)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::Hand,				DarkKnightAttackInfo(EDarkKnightAttackType::Hand,				FVector(70.0f, 100.0f, 100.0f), 1.2f, 0, 200)));
}

void EnemyDarkKnight::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	this->mStateManager.UpdateState(inDeltaTime);

	if (this->mStateManager.GetCurrentStateType() == EStateType::State_Attack)
	{
		this->mMovementComponent.Update(this->GetActorPtr(), 0.0f);
	}
	else
	{
		this->OnSyncLocation(inDeltaTime);
	}

	if (this->mStatsComponent.IsChanageStats(inDeltaTime))
	{
		this->DetectChangeEnemy();
	}

	const float debugDuration = 0.05f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), this->GetLocation(), 63.0f, debugDuration);
}

void EnemyDarkKnight::OnPatternShot(ActorPtr inVictim)
{
	this->OnMovementEnemy();
	int32 pattern = Random::GetIntUniformDistribution(0, static_cast<int32>(mPatternInfos.size() - 1));
	std::function<void(EnemyDarkKnight&)> pattenFunc = mPatternInfos[1];
	pattenFunc(*this);
}

void EnemyDarkKnight::OnPatternOver()
{
	if (this->IsDeath())
	{
		return;
	}

	if (false == this->BerserkPhase())
	{
		if (this->mStateManager.GetCurrentStateType() != EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Search);
		}
	}
}

void EnemyDarkKnight::OnStunWakeUp()
{
	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() == EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Search);
		}
	}
}

void EnemyDarkKnight::OnReward()
{
	DungeonPtr world = std::static_pointer_cast<Dungeon>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldNextTime = world->GetNextWorldTime();

	GameObjectPtr owner = this->GetOwner().lock();
	if (nullptr == owner)
	{
		return;
	}

	world->PushTask(worldNextTime, &GameWorld::DestroyActor, owner->GetGameObjectID());
}

void EnemyDarkKnight::OnHit(ActorPtr inInstigated, const float inDamage)
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
	const int64 worldTime = world->GetWorldTime();

	const float curHealth = this->mStatsComponent.GetCurrentStats().GetHealth() - inDamage;
	this->mStatsComponent.UpdateCurrentStat(EStatType::Stat_Health, curHealth);

	if (curHealth <= 0.0f)
	{
		this->mStateManager.SetState(EStateType::State_Death);
		return;
	}
	this->DetectChangeEnemy();

	{
		Protocol::S2C_HitEnemy hitPacket;
		hitPacket.set_object_id(this->GetGameObjectID());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, hitPacket);
		this->BrodcastPlayerViewers(sendBuffer);
	}
}

bool EnemyDarkKnight::BerserkPhase()
{

	if (mIsBerserkMode)
	{
		return false;
	}

	Stats curStats = this->GetEnemyStatsComponent().GetCurrentStats();
	Stats maxStats = this->GetEnemyStatsComponent().GetMaxStats();

	bool isOver = (0.4f >= (curStats.GetHealth() / maxStats.GetHealth()));
	if (false == isOver)
	{
		return false;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}
	const int64 worldTime = world->GetWorldTime();

	this->mBuffComponent.PushBuff(this->mStatsComponent, EBuffType::Buff_Berserk_AttackDamage, EStatType::Stat_AttackDamage, 100.0f);
	this->mBuffComponent.PushBuff(this->mStatsComponent, EBuffType::DeBuff_Berserk_Armor, EStatType::Stat_Armor, -30.0f);
	this->DetectChangeEnemy();

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Berserk));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

	this->PushTask(worldTime + 3000, &EnemyDarkKnight::OnPatternOver);
	mIsBerserkMode = true;
	return true;
}

void EnemyDarkKnight::RunningAttack()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	//TODO : 이동 채우기
	std::vector<MovePlane> planes;
	planes.push_back(MovePlane(426.0f, 2400));
	planes.push_back(MovePlane(-11.0f, 3000));
	planes.push_back(MovePlane(1.7f, 4030));
	planes.push_back(MovePlane(-73.7f, 5500));
	planes.push_back(MovePlane(5.0f, 6270));
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	const int64 lastTime = 6500 - 1500;
	this->PushTask(worldTime + 1500, &EnemyDarkKnight::DoMeleeNomalAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::Running), this->GetRotation(), lastTime);

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Running_Attack));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}

void EnemyDarkKnight::ChargedComboAttack()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	//TODO : 이동 채우기
	std::vector<MovePlane> planes;
	planes.push_back(MovePlane(-9.0f, 480));
	planes.push_back(MovePlane(37.0f, 1540));
	planes.push_back(MovePlane(-23.77f, 2200));
	planes.push_back(MovePlane(8.37f, 2970));
	planes.push_back(MovePlane(-7.93f, 4380));
	planes.push_back(MovePlane(3.33f, 5020));
	planes.push_back(MovePlane(-12.1f, 6330));
	planes.push_back(MovePlane(37.1f, 7230));
	planes.push_back(MovePlane(82.0f, 8400));
	planes.push_back(MovePlane(252.0f, 11460));
	planes.push_back(MovePlane(-8.0f, 12510));
	planes.push_back(MovePlane(-10.0f, 13240));
	planes.push_back(MovePlane(2.0f, 14100));
	planes.push_back(MovePlane(11.0f, 15100));
	planes.push_back(MovePlane(-21.0f, 16070));
	planes.push_back(MovePlane(11.0f, 17100));
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();

	this->PushTask(worldTime + 7300, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::RightToLeftSwing), this->GetRotation());
	this->PushTask(worldTime + 8350, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::LeftToRightSwing), this->GetRotation());
	this->PushTask(worldTime + 9720, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::UpperCut), this->GetRotation());

	int64 lastTime = 17100 - 11550;
	this->PushTask(worldTime + 11550, &EnemyDarkKnight::DoMeleeNomalAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::Slam), this->GetRotation(), lastTime);

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Charged_Combo_Attack));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}

void EnemyDarkKnight::UppercutAttack()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	//TODO : 이동 채우기
	std::vector<MovePlane> planes;
	planes.push_back(MovePlane(4.64f, 700));
	planes.push_back(MovePlane(-28.04f, 1970));
	planes.push_back(MovePlane(131.5f, 3900));
	planes.push_back(MovePlane(53.6f, 6400));
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 2900, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::UpperCut), this->GetRotation());

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Uppercut_Attack));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}

void EnemyDarkKnight::SwingAttack()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	std::vector<MovePlane> planes;
	planes.push_back(MovePlane(-12.0f, 1150));
	planes.push_back(MovePlane(+114.0f, 2800));
	planes.push_back(MovePlane(-22.0f, 3470));
	planes.push_back(MovePlane(+64.0f, 5870));
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 1950, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::RightToLeftSwing), this->GetRotation());

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Swing_Attack));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}

void EnemyDarkKnight::SwingAndSlamAttack()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	//TODO : 이동 채우기
	std::vector<MovePlane> planes;
	planes.push_back(MovePlane(-10.5f, 700));
	planes.push_back(MovePlane(116.4f, 2300));
	planes.push_back(MovePlane(-15.6f, 2940));
	planes.push_back(MovePlane(49.5f, 3720));
	planes.push_back(MovePlane(18.4f, 5100));
	planes.push_back(MovePlane(-9.2f, 5750));
	planes.push_back(MovePlane(12.5f, 6730));
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 1650, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::RightToLeftSwing), this->GetRotation());
	this->PushTask(worldTime + 3840, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::Slam), this->GetRotation());

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Swing_And_Slam_Attack));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}

void EnemyDarkKnight::HandAndSwordSwipeAttack()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	//TODO : 이동 채우기
	std::vector<MovePlane> planes;
	planes.push_back(MovePlane(-12.1f, 1000));
	planes.push_back(MovePlane(35.7f, 2270));
	planes.push_back(MovePlane(-6.4f, 2460));
	planes.push_back(MovePlane(73.5f, 3570));
	planes.push_back(MovePlane(99.8f, 4930));
	planes.push_back(MovePlane(-35.2f, 5730));
	planes.push_back(MovePlane(-1.8f, 6570));
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 2400, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::Hand), this->GetRotation());
	this->PushTask(worldTime + 3800, &EnemyDarkKnight::DoMeleeHardAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::LeftToRightSwing), this->GetRotation());

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Hand_And_Sword_Swipe_Attack));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}

void EnemyDarkKnight::ExterminationAttack()
{
	this->GameObjectLog(L"Skill_ExterminationAttack\n");

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	Location swordLocation = this->GetLocation() - (this->GetRotation().GetForwardVector() * 250.0f);

	ActorPtr actor = world->SpawnActor<Extermination>(this->GetGameObjectRef(), swordLocation, Rotation(), Scale(0.0f, 0.0f, 0.0f));
	std::shared_ptr<Extermination> newExplosion = std::static_pointer_cast<Extermination>(actor);
	if (nullptr == newExplosion)
	{
		return;
	}
	newExplosion->PushTask(worldTime + 5000, &Extermination::PushReserveDestroy);

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(this->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_DarkKnight_Uppercut_Attack));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);

}

void EnemyDarkKnight::DoMeleeHardAttack(DarkKnightAttackInfo inAttackInfo, Rotation inRotation)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	if (false == this->IsValid())
	{
		return;
	}

	Location location = this->GetLocation();
	Rotation rotation = inRotation;

	StatsComponent& stat = this->GetEnemyStatsComponent();
	float randomDamage = StatUtils::RandomDamage(stat.GetCurrentStats().GetAttackDamage()) * inAttackInfo.mMulDamage;

	ActorPtr actor = world->SpawnActor<EnemyMeleeAttack>(this->GetActorRef(), location, rotation, FVector(1.0f, 1.0f, 1.0f));
	if (nullptr == actor)
	{
		return;
	}

	std::shared_ptr<EnemyMeleeAttack> attack = std::static_pointer_cast<EnemyMeleeAttack>(actor);
	if (nullptr == attack)
	{
		return;
	}


	attack->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Hard_Melee);
	

	attack->SetTargetActorType(EActorType::Player);
	attack->SetAttackExtent(inAttackInfo.mExtent);
	attack->SetDamage(randomDamage);

	attack->PushTask(worldTime + inAttackInfo.mCheckCollisionTime, &EnemyMeleeAttack::CheackCollision);
}

void EnemyDarkKnight::DoMeleeNomalAttack(DarkKnightAttackInfo inAttackInfo, Rotation inRotation, int64 inDestroyTime)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	if (false == this->IsValid())
	{
		return;
	}

	Location location = this->GetLocation();
	Rotation rotation = inRotation;

	StatsComponent& stat = this->GetEnemyStatsComponent();
	float randomDamage = StatUtils::RandomDamage(stat.GetCurrentStats().GetAttackDamage()) * inAttackInfo.mMulDamage;

	ActorPtr actor = world->SpawnActor<EnemyMeleeAttack>(this->GetActorRef(), location, rotation, FVector(1.0f, 1.0f, 1.0f));
	if (nullptr == actor)
	{
		return;
	}

	std::shared_ptr<EnemyMeleeAttack> attack = std::static_pointer_cast<EnemyMeleeAttack>(actor);
	if (nullptr == attack)
	{
		return;
	}


	attack->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Melee);
	attack->SetTargetActorType(EActorType::Player);
	attack->SetAttackExtent(inAttackInfo.mExtent);
	attack->SetDamage(randomDamage);
	attack->SetParryinglTime(worldTime, worldTime + inAttackInfo.mParryingTime);

	if (inAttackInfo.mSkillID == EDarkKnightAttackType::Slam)
	{
		attack->mIsExtermination = true;
	}

	attack->PushTask(worldTime + inAttackInfo.mCheckCollisionTime, &EnemyMeleeAttack::CheackCollision);

	if (inDestroyTime != 0)
	{
		attack->PushTask(worldTime + inDestroyTime, &EnemyMeleeAttack::PushReserveDestroy);
	}
}

void EnemyDarkKnight::MakeMovePlane(const int64& inWorldTime, std::vector<MovePlane> inMovePlanes)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	float maxEstimatedDistnace = 0.0f;
	for (const MovePlane& plane : inMovePlanes)
	{
		maxEstimatedDistnace += plane.mDistance;
	}

	const FVector foward = this->GetRotation().GetForwardVector();
	const float radius = this->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();
	const float ranage = this->GetEnemyStatsComponent().GetCurrentStats().GetRange();

	const Location characterFoward = this->GetLocation() + (foward * ranage);
	Location estimatedDistnace = characterFoward + (foward * maxEstimatedDistnace);
	float ratio = 1.0f;

	LineTrace lineTrace(this->GetActorPtr(), characterFoward, estimatedDistnace, true);

	std::vector<FVector> impactLocation;
	bool result = world->FindObstructionIntersection(lineTrace, static_cast<uint8>(EActorType::Obstruction), impactLocation, 1);
	if (true == result)
	{
		//printf("DARK KNIGHT DISTANCE\n");
		//printf("%f\n", FVector::Distance2D(impactLocation.at(0), estimatedDistnace));
		//printf("%f\n", FVector::Distance2D(impactLocation.at(0), estimatedDistnace) / maxEstimatedDistnace);

		ratio -= FVector::Distance2D(impactLocation.at(0), estimatedDistnace) / maxEstimatedDistnace;
	}

	Location	originLocation	= this->GetLocation();
	Location	destinationLocation = Location();
	int64		originTime = inWorldTime;
	int64		destinationTime = 0;

	for(const MovePlane& plane : inMovePlanes)
	{

		destinationLocation = originLocation + (foward * (plane.mDistance * ratio));
		destinationLocation.SetZ(originLocation.GetZ());

		destinationTime += plane.mTime;

		this->PushTask(originTime, &EnemyDarkKnight::DoMoveLocation, originLocation, destinationLocation, destinationTime);

		originTime = inWorldTime + plane.mTime;
		destinationTime = -plane.mTime;

		originLocation = destinationLocation;
	}
}

void EnemyDarkKnight::DoMoveLocation(FVector inStartLocation, FVector inEndLocation, int64 inDuration)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	if (EStateType::State_Stun == this->GetStateManager().GetCurrentStateType())
	{
		return;
	}

	FRotator	direction = (inEndLocation - inStartLocation).Rotator();
	FVector		foward = direction.GetForwardVector();

	float		duration = inDuration / 1000.0f;

	float speed = (FVector::Distance(inStartLocation, inEndLocation)) / duration;
	this->SetVelocity(speed, speed, 0.0f);

	this->mMovementComponent.SetNewDestination(this->GetActorPtr(), inStartLocation, inEndLocation, worldTime, 0.0f);

	Protocol::S2C_AnimationMovementEnemy animationMovementPacket;
	animationMovementPacket.set_object_id(this->GetGameObjectID());
	animationMovementPacket.mutable_start_location()->CopyFrom(PacketUtils::ToSVector(inStartLocation));
	animationMovementPacket.mutable_end_location()->CopyFrom(PacketUtils::ToSVector(inEndLocation));
	animationMovementPacket.set_duration(inDuration);
	animationMovementPacket.set_timestamp(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, animationMovementPacket);
	this->BrodcastPlayerViewers(appearSendBuffer);
}
