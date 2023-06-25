#include "pch.h"
#include "EnemyCharacter.h"

EnemyCharacter::EnemyCharacter(const WCHAR* inName) : Pawn(inName), mEnemyID(0), mSpawnObjectID(0)
{
}

EnemyCharacter::~EnemyCharacter()
{
}

void EnemyCharacter::Initialization()
{
}

void EnemyCharacter::Destroy()
{
}

void EnemyCharacter::Tick()
{
	if (true == IsAttackRange())
	{

	}
}

bool EnemyCharacter::IsAttackRange()
{
	if (false == IsValid())
	{
		return false;
	}

	mStats.GetRange();

	return false;
}

void EnemyCharacter::SetEnemeyID(const int32 inEnemyID)
{
	mEnemyID = inEnemyID;
}

void EnemyCharacter::SetSpawnObjectID(const int64 inSpawnObjectID)
{
	mSpawnObjectID = inSpawnObjectID;
}

void EnemyCharacter::SetStats(const Stats& inStats)
{
	mStats = inStats;
}

void EnemyCharacter::SetSpawnLocation(const Protocol::SVector& inSpawnLocation)
{
	mSpawnLocation = inSpawnLocation;
}

const Protocol::SEnemy EnemyCharacter::ConvertSEnemy()
{
	Protocol::SEnemy tempEnemy;
	tempEnemy.set_object_id(this->GetGameObjectID());
	tempEnemy.set_enemy_id(this->GetEnemyID());
	tempEnemy.mutable_location()->CopyFrom(GetLocation());
	wprintf(L"[%lld][%ld] (%f:%f:%f)", this->GetGameObjectID(), this->GetEnemyID(), GetLocation().x(), GetLocation().y(), GetLocation().z());
	return tempEnemy;
}
