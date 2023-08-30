#include "pch.h"
#include "EnemyDarkKnight.h"

EnemyDarkKnight::EnemyDarkKnight() : EnemyCharacter(L"DarkKnight")
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

	this->SetAggressive(true);

	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Search);

	this->mStatsComponent.SetSyncTime(GAME_TICK);

	BoxCollisionComponent* collision = this->GetCapsuleCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(42.0f, 42.0f, 96.0f));

	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());


	this->mPatternInfos.push_back(&EnemyDarkKnight::RunningAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::ChargedComboAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::UppercutAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::SwingAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::SwingAndSlamAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::HandAndSwordSwipeAttack);
	this->mPatternInfos.push_back(&EnemyDarkKnight::Berserk);

}

void EnemyDarkKnight::OnPatternShot(ActorPtr inVictim)
{
	int32 index = Random::GetIntUniformDistribution(0, static_cast<int32>(mPatternInfos.size()));

	std::function<void(EnemyDarkKnight&)> pattenFunc = mPatternInfos[index];
	pattenFunc(*this);
}

void EnemyDarkKnight::RunningAttack()
{
	printf("RunningAttack\n");
}

void EnemyDarkKnight::ChargedComboAttack()
{
	printf("ChargedComboAttack\n");
}

void EnemyDarkKnight::UppercutAttack()
{
	printf("UppercutAttack\n");
}

void EnemyDarkKnight::SwingAttack()
{
	printf("SwingAttack\n");
}

void EnemyDarkKnight::SwingAndSlamAttack()
{
	printf("SwingAndSlamAttack\n");
}

void EnemyDarkKnight::HandAndSwordSwipeAttack()
{
	printf("HandAndSwordSwipeAttack\n");
}

void EnemyDarkKnight::Berserk()
{
	printf("Berserk\n");
}
