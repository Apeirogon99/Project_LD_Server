#include "pch.h"
#include "WarriorSwordBlow.h"

WarriorSwordBlow::WarriorSwordBlow() : ActiveSkill(L"WarriorSwordBlow"), mIsCharge(false), mChargeVelocity(100.0f), mMaxChargeDuration(3.0f), mDefaultChargeDuration(1.0f)
{
}

WarriorSwordBlow::~WarriorSwordBlow()
{
}

void WarriorSwordBlow::OnInitialization()
{
	SetTick(true, SYSTEM_TICK);
}

void WarriorSwordBlow::OnDestroy()
{
    Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
    disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

    SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
    this->BrodcastPlayerViewers(sendBuffer);
}

void WarriorSwordBlow::OnTick(const int64 inDeltaTime)
{

	if (false == IsValid())
	{
		return;
	}

	if (false == mIsCharge)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	//printf("CHARGE : %f\n", (world->GetWorldTime() - this->mActiveTime) / 1000.0f);

}

bool WarriorSwordBlow::IsValid()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{

		if(false == world->IsValidActor(this->GetGameObjectID()))
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

void WarriorSwordBlow::Active()
{
    if (false == mIsCharge)
    {
        mIsCharge = true;
		return;
	}
	else
	{
		mIsCharge = false;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& endChargeTime = world->GetWorldTime();
	float chargeDuration = (endChargeTime - this->mActiveTime) / 1000.0f;
	chargeDuration = (chargeDuration >= mMaxChargeDuration) ? mMaxChargeDuration : (chargeDuration >= mDefaultChargeDuration) ? chargeDuration : mDefaultChargeDuration;

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

	FVector boxExtent(chargeDuration * this->mChargeVelocity,100.0f, 100.0f);

	FVector		location = instigated->GetMovementComponent().GetCurrentLocation(instigated->GetActorPtr());
	FRotator	rotation = instigated->GetRotation();
	FVector		foward	= rotation.GetForwardVector();
	const float collision = instigated->GetCapsuleCollisionComponent()->GetBoxCollision().GetBoxExtent().GetX();
	const float radius = std::sqrtf(std::powf(boxExtent.GetX(), 2) + std::powf(boxExtent.GetY(), 2));	//외접원 반지름

	Location boxStartLocation	= location + (foward * collision);
	Location boxEndLocation		= boxStartLocation + (foward * (boxExtent.GetX() * 2));
	Location boxCenterLocation	= (boxStartLocation + boxEndLocation) / 2.0f;
	BoxTrace boxTrace(instigated->GetActorRef(), boxStartLocation, boxEndLocation, true, boxExtent, rotation);

	//DEBUG
	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawBox(this->GetPlayerViewers(), boxStartLocation, boxEndLocation, boxExtent, debugDuration);
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), boxCenterLocation, radius, debugDuration);


	std::vector<ActorPtr> findActors;
	uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
	bool result = world->FindActors(boxTrace, findActorType, findActors);
	if (!result)
	{
		return;
	}

	for (ActorPtr actor : findActors)
	{
		EnemyCharacterPtr enemy = std::static_pointer_cast<EnemyCharacter>(actor);
		if (nullptr == enemy)
		{
			continue;
		}

		enemy->PushTask(endChargeTime, &Actor::OnHit, instigated->GetActorPtr(), 100.0f);

	}

	Protocol::S2C_ReactionSkill reactionSkill;
	reactionSkill.set_remote_id(owner->GetGameObjectID());
	reactionSkill.set_object_id(this->GetGameObjectID());
	reactionSkill.set_skill_id(this->GetSkillID());
	reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	reactionSkill.set_duration(endChargeTime - this->mActiveTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
	this->BrodcastPlayerViewers(sendBuffer);
}

const bool& WarriorSwordBlow::IsCharge() const
{
	return mIsCharge;
}
