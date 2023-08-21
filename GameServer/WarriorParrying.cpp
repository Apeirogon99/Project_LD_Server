#include "pch.h"
#include "WarriorParrying.h"

WarriorParrying::WarriorParrying() : ActiveSkill(L"WarriorParrying"), mIsParrying(false), mIsHit(false), mStartParryingTime(200), mEndParryingTime(2000)
{
}

WarriorParrying::~WarriorParrying()
{
}

void WarriorParrying::OnInitialization()
{
}

void WarriorParrying::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void WarriorParrying::OnTick(const int64 inDeltaTime)
{
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
	
	if (true == mIsHit)
	{
		return;
	}
	mIsHit = true;

	if (false == mIsParrying)
	{
		return;
	}

	printf("WarriorParrying Success\n");

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

	Protocol::S2C_ReactionSkill reactionSkill;
	reactionSkill.set_remote_id(owner->GetGameObjectID());
	reactionSkill.set_object_id(this->GetGameObjectID());
	reactionSkill.set_skill_id(this->GetSkillID());
	reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	reactionSkill.set_duration(worldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
	this->BrodcastPlayerViewers(sendBuffer);

	this->DeActive(1000);
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
	const float collision = instigated->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();
	const float radius = std::sqrtf(std::powf(boxExtent.GetX(), 2) + std::powf(boxExtent.GetY(), 2));	//외접원 반지름

	Location boxStartLocation = location + (foward * collision);
	Location boxEndLocation = boxStartLocation + (foward * (boxExtent.GetX() * 2));
	Location boxCenterLocation = (boxStartLocation + boxEndLocation) / 2.0f;
	BoxTrace boxTrace(this->GetActorRef(), boxStartLocation, boxEndLocation, true, boxExtent, rotation);

	this->PushTask(worldTime + mStartParryingTime, &WarriorParrying::StartParrying);
	this->PushTask(worldTime + mEndParryingTime, &WarriorParrying::EndParrying);

}

void WarriorParrying::StartParrying()
{
	mIsParrying = true;

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
	const float collision = instigated->GetCapsuleCollisionComponent().GetBoxCollision().GetBoxExtent().GetX();
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

	if (false == mIsHit)
	{
		this->DeActive(1000);
	}

}

void WarriorParrying::SetWarriorParrying(const float& inDamage)
{
}
