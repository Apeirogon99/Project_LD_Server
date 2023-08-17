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
	chargeDuration = (chargeDuration >= mMaxChargeDuration) ? mMaxChargeDuration : (chargeDuration >= mDefaultChargeDuration) ? chargeDuration : mDefaultChargeDuration;

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerCharacterPtr instigated = remotePlayer->GetCharacter();
	if (nullptr == instigated)
	{
		return;
	}

	FRotator rotation = this->GetRotation();
	FVector fowrad = rotation.GetForwardVector();

	FVector start = this->GetLocation();
	FVector end = start + (fowrad * chargeDuration * mChargeVelocity);
	FVector mid = start - (start - end) / 2;
	FVector extent = FVector(50.0f, chargeDuration * mChargeVelocity, 50.0f);

	BoxTrace boxTrace(mid, mid, false, extent, rotation);

	const float radius = (0.5f * std::sqrtf(std::powf(extent.GetX(), 2) + std::powf(extent.GetY(), 2)));	//외접원 반지름

	std::vector<ActorPtr> findActors;
	uint8 findActorType = static_cast<uint8>(EActorType::Enemy);
	bool result = world->FindActors(mid, radius, findActorType, findActors);
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
			enemy->PushTask(endChargeTime, &Actor::OnHit, instigated->GetActorPtr(), 100.0f);
		}

	}

}

const bool& WarriorSwordBlow::IsCharge() const
{
	return mIsCharge;
}
