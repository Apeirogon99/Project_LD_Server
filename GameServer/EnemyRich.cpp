#include "pch.h"
#include "EnemyRich.h"

//==========================//
//       Rich | Rich		//
//==========================//

EnemyRich::EnemyRich(const WCHAR* inName) : EnemyCharacter(inName)
{
}

EnemyRich::~EnemyRich()
{
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
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	this->mPatternInfos.push_back(&EnemyRichPhase1::RiseSkeleton);
	this->mPatternInfos.push_back(&EnemyRichPhase1::BlinkAttack);
	this->mPatternInfos.push_back(&EnemyRichPhase1::Explosion);
	this->mPatternInfos.push_back(&EnemyRichPhase1::MultiCasting);
}

void EnemyRichPhase1::OnPatternShot(ActorPtr inVictim)
{
}

void EnemyRichPhase1::RiseSkeleton()
{
}

void EnemyRichPhase1::BlinkAttack()
{
}

void EnemyRichPhase1::Explosion()
{
}

void EnemyRichPhase1::MultiCasting()
{
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
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);

	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	this->mPatternInfos.push_back(&EnemyRichPhase2::BlinkSturn);
	this->mPatternInfos.push_back(&EnemyRichPhase2::SoulSpark);
	this->mPatternInfos.push_back(&EnemyRichPhase2::SoulShackles);
}

void EnemyRichPhase2::OnPatternShot(ActorPtr inVictim)
{
}

void EnemyRichPhase2::RiseDarkKnight()
{
}

void EnemyRichPhase2::BlinkSturn()
{
}

void EnemyRichPhase2::SoulSpark()
{
}

void EnemyRichPhase2::SoulShackles()
{
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

	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());


	this->mPatternInfos.push_back(&EnemyRichPhase3::RiseSkeleton);
	this->mPatternInfos.push_back(&EnemyRichPhase3::OnslaughtOfShadows);

}

void EnemyRichPhase3::OnPatternShot(ActorPtr inVictim)
{
}

void EnemyRichPhase3::RiseSkeleton()
{
}

void EnemyRichPhase3::RealmOfDeath()
{
}

void EnemyRichPhase3::OnslaughtOfShadows()
{
}

void EnemyRichPhase3::LifeVessel()
{
}
