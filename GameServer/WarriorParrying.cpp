#include "pch.h"
#include "WarriorParrying.h"

WarriorParrying::WarriorParrying() : ActiveSkill(L"WarriorParrying"), mIsParrying(false), mStartParryingTime(200), mEndParryingTime(300), mSturnDuration(3000), mParryingDamage(0.2f)
{
}

WarriorParrying::~WarriorParrying()
{
}

void WarriorParrying::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	SetTick(true, SYSTEM_TICK);

	this->BeginCastingSkill();

	this->PushTask(worldTime + mStartParryingTime, &WarriorParrying::StartParrying);
	this->PushTask(worldTime + mEndParryingTime, &WarriorParrying::EndParrying);
}

void WarriorParrying::OnDestroy()
{

	this->EndCastingSkill();

	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void WarriorParrying::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	if (mIsParrying)
	{
		this->Active();
	}
}

bool WarriorParrying::IsValid()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		if (false == world->IsValidActor(this->GetGameObjectID()))
		{
			return false;
		}

		bool ret = world->DestroyActor(this->GetGameObjectID());
		if (false == ret)
		{
			this->GameObjectLog(L"Can't destroy skill\n");
		}
		return ret;
	}

	return true;
}

void WarriorParrying::OnHit(ActorPtr inInstigated, const float inDamage)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	GameRemotePlayerPtr owner = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	PlayerCharacterPtr player = owner->GetCharacter();

	if (false == mIsParrying)
	{
		float currentHP = player->GetStatComponent().GetCurrentStats().GetHealth();
		player->GetStatComponent().UpdateCurrentStat(EStatType::Stat_Health, currentHP - inDamage);
		return;
	}

}

void WarriorParrying::Active()
{

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	GameRemotePlayerPtr owner = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	PlayerCharacterPtr instigated = owner->GetCharacter();
	if (nullptr == instigated)
	{
		return;
	}

	FVector boxExtent(30.0f, 80.0f, 100.0f);

	FVector		location = instigated->GetMovementComponent().GetCurrentLocation(instigated->GetActorPtr());
	FRotator	rotation = this->GetRotation();
	FVector		foward = rotation.GetForwardVector();
	const float collision = instigated->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();
	const float radius = std::sqrtf(std::powf(boxExtent.GetX(), 2) + std::powf(boxExtent.GetY(), 2));	//외접원 반지름

	Location boxStartLocation = location + (foward * collision);
	Location boxEndLocation = boxStartLocation + (foward * (boxExtent.GetX() * 2));
	BoxTrace boxTrace(this->GetActorRef(), boxStartLocation, boxEndLocation, true, boxExtent, rotation);

	uint8 findActorType = static_cast<uint8>(EActorType::EnemyAttack);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(boxTrace, findActorType, findActors, 1);
	if (false == result)
	{
		return;
	}

	for (auto actor : findActors)
	{
		EnemyAttackPtr enemyAttack = std::static_pointer_cast<EnemyAttack>(actor);
		if (nullptr == enemyAttack)
		{
			continue;
		}
		const EEnemyAttackType& attackType = enemyAttack->GetEnemyAttackType();
		const float attackDamage = enemyAttack->GetDamage() * mParryingDamage;

		EnemyCharacterPtr enemyOwner = std::static_pointer_cast<EnemyCharacter>(actor->GetOwner().lock());
		if (nullptr == enemyOwner)
		{
			continue;
		}

		const Rotation& enemyRot		= enemyOwner->GetRotation();
		const float enemyMinRot			= fmodf(enemyRot.GetYaw() - 20, 180);
		const float enemyMaxRot			= fmodf(enemyRot.GetYaw() + 20, 180);

		const Rotation& instigatedTRot	= FRotator::TurnRotator(instigated->GetRotation());

		if (enemyMinRot > instigatedTRot.GetYaw() || instigatedTRot.GetYaw() > enemyMaxRot)
		{
			continue;
		}

		if (attackType == EEnemyAttackType::Enemy_Attack_Nomal_Melee)
		{
			if (false == enemyAttack->OnParrying(instigated))
			{
				continue;
			}

			enemyOwner->GetStateManager().SetState(EStateType::State_Stun);
			StunState* stunState = static_cast<StunState*>(enemyOwner->GetStateManager().GetCurrentStateEvent());
			if (nullptr == stunState)
			{
				continue;
			}

			stunState->SetStunTime(mSturnDuration);
			enemyOwner->OnHit(instigated, attackDamage);

		}
		else if (attackType == EEnemyAttackType::Enemy_Attack_Nomal_Projectile)
		{
			enemyAttack->SetDamage(attackDamage);
			enemyAttack->SetTargetActorType(EActorType::Enemy);

			enemyAttack->OnParrying(instigated);
		}

		Protocol::S2C_ReactionSkill reactionSkill;
		reactionSkill.set_remote_id(owner->GetGameObjectID());
		reactionSkill.set_object_id(this->GetGameObjectID());
		reactionSkill.set_skill_id(this->GetSkillID());
		reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(enemyAttack->GetLocation()));
		reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(enemyAttack->GetRotation()));
		reactionSkill.set_duration(worldTime);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
		this->BrodcastPlayerViewers(sendBuffer);

		world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
		return;
	}

}

void WarriorParrying::StartParrying()
{
	mIsParrying = true;

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	GameRemotePlayerPtr owner = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	PlayerCharacterPtr instigated = owner->GetCharacter();
	if (nullptr == instigated)
	{
		return;
	}

	FVector boxExtent(30.0f, 80.0f, 100.0f);

	FVector		location = this->GetLocation();
	FRotator	rotation = this->GetRotation();
	FVector		foward = rotation.GetForwardVector();
	const float collision = instigated->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();
	const float radius = std::sqrtf(std::powf(boxExtent.GetX(), 2) + std::powf(boxExtent.GetY(), 2));	//외접원 반지름

	Location boxStartLocation = location + (foward * collision);
	Location boxEndLocation = boxStartLocation + (foward * (boxExtent.GetX() * 2));
	Location boxCenterLocation = (boxStartLocation + boxEndLocation) / 2.0f;
	BoxTrace boxTrace(this->GetActorRef(), boxStartLocation, boxEndLocation, true, boxExtent, rotation);

	PacketUtils::DebugDrawBox(this->GetPlayerViewers(), boxStartLocation, boxEndLocation, boxExtent, 1.0f);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), boxCenterLocation, radius, 1.0f);

}

void WarriorParrying::EndParrying()
{
	mIsParrying = false;

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	const int64& nextWorldTime = world->GetNextWorldTime();

	if(false == world->IsValidActor(this->GetGameObjectID()))
	{
		return;
	}

	//this->PushTask(worldTime + 700, &ActiveSkill::EndCastingSkill);
	world->PushTask(nextWorldTime + 700, &GameWorld::DestroyActor, this->GetGameObjectID());
}

void WarriorParrying::SetWarriorParrying(const float& inDamage)
{
}
