#include "pch.h"
#include "WarriorSwordBlow.h"

WarriorSwordBlow::WarriorSwordBlow() : ActiveSkill(L"WarriorSwordBlow"), mIsCharge(false), mChargeVelocity(100.0f, 100.0f, 100.0f), mMaxChargeDuration(3000.0f)
{
}

WarriorSwordBlow::~WarriorSwordBlow()
{
}

void WarriorSwordBlow::OnInitialization()
{
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

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& endChargeTime = world->GetWorldTime();
	float chargeDuration = (endChargeTime - this->mActiveTime) / 1000.0f;
	chargeDuration = (chargeDuration >= mMaxChargeDuration) ? mMaxChargeDuration : chargeDuration;

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return;
	}

	FRotator rotation = this->GetRotation();
	FVector fowrad = this->GetRotation().GetForwardVector();

	FVector start = this->GetLocation();
	FVector end = start + (fowrad * chargeDuration * mChargeVelocity);
	FVector extent = FVector(100.0f, 100.0f, 100.0f);
	const float radius = (0.5f * std::sqrtf(std::powf(extent.GetX(), 2) + std::powf(extent.GetY(), 2)));	//외접원 반지름

	BoxTrace boxTrace(start, end, false, extent, rotation);

	std::vector<ActorPtr> findActors;
	uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
	bool result = world->FindActors((start - end) / 2, radius, findActorType, findActors);
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

		bool isOverlap = boxTrace.BoxCollisionTrace(enemy->GetCapsuleCollisionComponent());
		if (isOverlap)
		{
			enemy->PushTask(endChargeTime, &Actor::OnHit, this->GetActorPtr(), 100.0f);
		}

	}

}

const bool& WarriorSwordBlow::IsCharge() const
{
	return mIsCharge;
}
