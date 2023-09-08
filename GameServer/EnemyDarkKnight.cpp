#include "pch.h"
#include "EnemyDarkKnight.h"

DarkKnightAttackInfo::DarkKnightAttackInfo() : mExtent(), mMulDamage(0.0f), mParryingTime(0), mCheckCollisionTime(0)
{
}

DarkKnightAttackInfo::DarkKnightAttackInfo(const FVector& inExtent, const float& inDamage, const int64& inParryingTime, const int64& inCheckCollisionTime)
{
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

	this->SetEnemeyID(static_cast<int32>(EnemyID::Enemy_Dark_Knight));
	this->SetAggressive(true);

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (datas)
	{
		this->mStatsComponent.SetSyncTime(GAME_TICK);
		this->mStatsComponent.InitMaxStats(datas->GetEnemyStat(static_cast<int32>(EnemyID::Enemy_Dark_Knight)));
	}

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
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::Running,			DarkKnightAttackInfo(FVector(100.0f, 100.0f, 100.0f), 1.0f, 100, 200)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::UpperCut,			DarkKnightAttackInfo(FVector(100.0f, 100.0f, 100.0f), 1.0f,   0, 200)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::LeftToRightSwing,	DarkKnightAttackInfo(FVector(100.0f, 100.0f, 100.0f), 1.0f,   0, 200)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::RightToLeftSwing,	DarkKnightAttackInfo(FVector(100.0f, 100.0f, 100.0f), 1.0f,   0, 200)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::Slam,				DarkKnightAttackInfo(FVector(100.0f, 100.0f, 100.0f), 1.0f,   0, 200)));
	this->mDarkKnightAttacks.insert(std::make_pair(EDarkKnightAttackType::Hand,				DarkKnightAttackInfo(FVector(100.0f, 100.0f, 100.0f), 1.0f,   0, 200)));
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

	const float debugDuration = 0.5f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), this->GetLocation(), 63.0f, debugDuration);

	this->GetLocation().ToString();
}

void EnemyDarkKnight::OnPatternShot(ActorPtr inVictim)
{
	int32 pattern = Random::GetIntUniformDistribution(0, static_cast<int32>(mPatternInfos.size() - 1));
	std::function<void(EnemyDarkKnight&)> pattenFunc = mPatternInfos[3];
	pattenFunc(*this);
}

void EnemyDarkKnight::OnPatternOver()
{
	if (this->IsDeath())
	{
		return;
	}

	this->mStateManager.SetState(EStateType::State_Search);

}

void EnemyDarkKnight::OnReward()
{
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

	this->BerserkPhase();

	if (curHealth <= 0.0f)
	{
		this->mStateManager.SetState(EStateType::State_Death);
		return;
	}

	if (this->mStateManager.GetCurrentStateType() != EStateType::State_Attack && this->mStateManager.GetCurrentStateType() != EStateType::State_Stun)
	{
		this->mStateManager.SetState(EStateType::State_Hit);
	}
	else
	{
		this->DetectChangeEnemy();
	}
}

void EnemyDarkKnight::OnDeath()
{
}

void EnemyDarkKnight::BerserkPhase()
{

	if (mIsBerserkMode)
	{
		return;
	}

	Stats curStats = this->GetEnemyStatsComponent().GetCurrentStats();
	Stats maxStats = this->GetEnemyStatsComponent().GetMaxStats();

	bool isOver = 0.4 >= (curStats.GetHealth() / maxStats.GetHealth());
	if (isOver)
	{
		mIsBerserkMode = true;
		this->Berserk();
	}

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
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 1870, &EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::Running));
	this->PushTask(worldTime + 6270, &EnemyDarkKnight::OnPatternOver);

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
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 7500,	&EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::RightToLeftSwing));
	this->PushTask(worldTime + 8550,	&EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::LeftToRightSwing));
	this->PushTask(worldTime + 9920,	&EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::UpperCut));
	this->PushTask(worldTime + 11750,	&EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::Slam));
	this->PushTask(worldTime + 17100,	&EnemyDarkKnight::OnPatternOver);

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
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 3050, &EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::UpperCut));
	this->PushTask(worldTime + 6400, &EnemyDarkKnight::OnPatternOver);

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
	planes.push_back(MovePlane(-12.0f,	1150));
	planes.push_back(MovePlane(+114.0f, 2800));
	planes.push_back(MovePlane(-22.0f,	3470));
	planes.push_back(MovePlane(+64.0f,	5870));
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 2190, &EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::RightToLeftSwing));
	this->PushTask(world->GetNextWorldTime() + 5870, &EnemyDarkKnight::OnPatternOver);

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
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 1790, &EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::RightToLeftSwing));
	this->PushTask(worldTime + 3980, &EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::Slam));
	this->PushTask(worldTime + 6730, &EnemyDarkKnight::OnPatternOver);

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
	this->MakeMovePlane(worldTime, planes);

	ActorPtr targetActor = this->GetAggroActor().lock();
	this->PushTask(worldTime + 2500, &EnemyDarkKnight::DoMeleeAttack,mDarkKnightAttacks.at(EDarkKnightAttackType::Hand));
	this->PushTask(worldTime + 3900, &EnemyDarkKnight::DoMeleeAttack, mDarkKnightAttacks.at(EDarkKnightAttackType::LeftToRightSwing));
	this->PushTask(worldTime + 6570, &EnemyDarkKnight::OnPatternOver);

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

void EnemyDarkKnight::Berserk()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	this->mBuffComponent.PushBuff(this->mStatsComponent, EStatType::Stat_AttackDamage, 100.0f);
	this->mBuffComponent.PushBuff(this->mStatsComponent, EStatType::Stat_Armor, -30.0f);
	this->DetectChangeEnemy();

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

void EnemyDarkKnight::DoMeleeAttack(DarkKnightAttackInfo inAttackInfo)
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
	Rotation rotation = this->GetRotation();

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

	if (inAttackInfo.mParryingTime > 0)
	{
		attack->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Melee);
	}
	else
	{
		attack->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Hard_Melee);
	}

	attack->SetTargetActorType(EActorType::Player);
	attack->SetAttackExtent(inAttackInfo.mExtent);
	attack->SetDamage(randomDamage);
	attack->SetParryinglTime(worldTime, worldTime + inAttackInfo.mParryingTime);

	attack->PushTask(worldTime + inAttackInfo.mCheckCollisionTime, &EnemyMeleeAttack::CheackCollision);
	world->PushTask(world->GetNextWorldTime() + inAttackInfo.mCheckCollisionTime, &GameWorld::DestroyActor, attack->GetGameObjectID());
}

void EnemyDarkKnight::MakeMovePlane(const int64& inWorldTime, std::vector<MovePlane> inMovePlanes)
{
	FVector	foward = this->GetRotation().GetForwardVector();

	Location	originLocation	= this->GetLocation();
	Location	destinationLocation = Location();
	int64		originTime = inWorldTime;
	int64		destinationTime = 0;

	for(const MovePlane& plane : inMovePlanes)
	{

		destinationLocation = originLocation + (foward * plane.mDistance);
		destinationLocation.SetZ(originLocation.GetZ());

		destinationTime = plane.mTime;

		this->PushTask(originTime, &EnemyDarkKnight::DoMoveLocation, originLocation, destinationLocation, destinationTime);

		originTime += destinationTime;
		destinationTime = -destinationTime;

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
