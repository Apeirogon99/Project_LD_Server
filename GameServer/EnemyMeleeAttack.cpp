#include "pch.h"
#include "EnemyMeleeAttack.h"

EnemyMeleeAttack::EnemyMeleeAttack() : EnemyAttack(L"EnemyAttack::Melee"), mIsExtermination(false)
{
	this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

EnemyMeleeAttack::~EnemyMeleeAttack()
{
}

void EnemyMeleeAttack::OnInitialization()
{
	this->SetActorType(static_cast<uint8>(EActorType::EnemyAttack));

	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
	collision->SetOwner(this->GetActorRef());
}

void EnemyMeleeAttack::OnDestroy()
{
}

void EnemyMeleeAttack::OnTick(const int64 inDeltaTime)
{
}

bool EnemyMeleeAttack::IsValid()
{
	return false;
}

void EnemyMeleeAttack::CheackCollision()
{

	if (mIsParrying)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetNextWorldTime();

	EnemyCharacterPtr owner = std::static_pointer_cast<EnemyCharacter>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}
	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();

	FVector		location	= this->GetLocation();
	FRotator	rotation	= this->GetRotation();
	FVector		foward		= rotation.GetForwardVector();

	if (mEnemyAttackType == EEnemyAttackType::Enemy_Attack_Hard_Melee)
	{
		world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
	}

	if (mIsExtermination)
	{
		owner->ExterminationAttack();
		world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
	}
	else
	{
		FVector		boxExtent = collision->GetBoxCollision().GetBoxExtent();
		const float radius = std::sqrtf(std::powf(boxExtent.GetX(), 2) + std::powf(boxExtent.GetY(), 2));	//외접원 반지름

		Location boxStartLocation = location;
		Location boxEndLocation = boxStartLocation + (foward * (boxExtent.GetX() * 2));
		Location boxCenterLocation = (boxStartLocation + boxEndLocation) / 2.0f;
		BoxTrace boxTrace(this->GetActorRef(), boxStartLocation, boxEndLocation, true, boxExtent, rotation);

		//DEBUG
		const float debugDuration = 1.0f;
		PacketUtils::DebugDrawBox(owner->GetPlayerViewers(), boxStartLocation, boxEndLocation, boxExtent, debugDuration);
		PacketUtils::DebugDrawSphere(owner->GetPlayerViewers(), boxCenterLocation, radius, debugDuration);

		uint8 findActorType = static_cast<uint8>(EActorType::Player);
		std::vector<ActorPtr> findActors;
		bool result = world->FindActors(boxTrace, findActorType, findActors);
		if (!result)
		{
			return;
		}

		for (ActorPtr actor : findActors)
		{

			PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(actor);
			if (nullptr == character)
			{
				continue;
			}

			character->PushTask(worldTime, &Actor::OnHit, this->GetActorPtr(), this->GetDamage());

		}
	}
}

bool EnemyMeleeAttack::OnParrying(ActorPtr inActor)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}

	if (false == this->CanParrying())
	{
		return false;
	}

	EnemyCharacterPtr owner = std::static_pointer_cast<EnemyCharacter>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return false;
	}
	owner->GetMovementComponent().SetNewDestination(this->GetActorPtr(), owner->GetLocation(), owner->GetLocation(), world->GetWorldTime(), 0.0f);
	owner->OnMovementEnemy();

	bool ret = world->DestroyActor(this->GetGameObjectID());
	if (false == ret)
	{
		this->GameObjectLog(L"Can't destroy parrying\n");
		return false;
	}

	mIsParrying = true;

	return true;
}

void EnemyMeleeAttack::SetAttackExtent(const FVector inExtent)
{
	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
	collision->SetBoxCollision(inExtent);
}

BoxCollisionComponent* EnemyMeleeAttack::GetBoxCollisionComponent() const
{
	return static_cast<BoxCollisionComponent*>(this->mDefaultCollisionComponent);
}
