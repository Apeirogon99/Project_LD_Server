#include "pch.h"
#include "Blink.h"

//==========================//
//          Blink			//
//==========================//

Blink::Blink(const WCHAR* inName) : EnemyAttack(inName), mStartTime(0), mBlinkID(0)
{
	this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

Blink::~Blink()
{
}

void Blink::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	mStartTime = world->GetWorldTime();


	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(50.0f, 80.0f, 100.0f));
}

void Blink::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void Blink::OnTick(const int64 inDeltaTime)
{
}

bool Blink::IsValid()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
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

void Blink::OnAppearActor(ActorPtr inAppearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inAppearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == IsValid())
	{
		return;
	}

	if (mPlayerViewers.find(anotherPlayerState) != mPlayerViewers.end())
	{
		return;
	}
	this->InsertPlayerViewer(anotherPlayerState);
	anotherPlayerState->InsertActorMonitor(this->GetActorPtr());

	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(owner->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(this->mBlinkID);
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime - this->mStartTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void Blink::OnDisAppearActor(ActorPtr inDisappearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inDisappearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == this->FindPlayerViewer(anotherPlayerState))
	{
		return;
	}
	this->ReleasePlayerViewer(anotherPlayerState);
	anotherPlayerState->ReleaseActorMonitor(this->GetActorPtr());

	Protocol::S2C_DisAppearGameObject disAppearItemPacket;
	disAppearItemPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disAppearItemPacket);
	anotherPlayerState->Send(appearItemSendBuffer);
}

void Blink::SetBlinkID(const ESkillID& inSkillID)
{
	mBlinkID = static_cast<int32>(inSkillID);
}

void Blink::TeleportPlayerLocation(const FVector inLocation, const FRotator inRotation)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	std::shared_ptr<EnemyRich> rich = std::static_pointer_cast<EnemyRich>(this->GetOwner().lock());
	if (nullptr == rich)
	{
		return;
	}

	const Location& destination = inLocation + (inRotation.GetForwardVector() * 100);
	rich->GetMovementComponent().SetNewDestination(owner, inLocation, destination, worldTime, 0.0f);

	Protocol::S2C_Teleport teleportPacket;
	teleportPacket.set_object_id(owner->GetGameObjectID());
	teleportPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(inLocation));

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, teleportPacket);
	this->BrodcastPlayerViewers(sendBuffer);

}

void Blink::TeleportSafeLocation(const FVector inLocation, const FRotator inRotation)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	std::shared_ptr<EnemyRich> rich = std::static_pointer_cast<EnemyRich>(this->GetOwner().lock());
	if (nullptr == rich)
	{
		return;
	}

	const Location& destination = inLocation + (inRotation.GetForwardVector() * 100);
	rich->GetMovementComponent().SetNewDestination(owner, inLocation, destination, worldTime, 0.0f);

	Protocol::S2C_Teleport teleportPacket;
	teleportPacket.set_object_id(owner->GetGameObjectID());
	teleportPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(inLocation));

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, teleportPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

BoxCollisionComponent* Blink::GetBoxCollisionComponent() const
{
	return static_cast<BoxCollisionComponent*>(this->mDefaultCollisionComponent);
}

//==========================//
//       BlinkAttack		//
//==========================//

BlinkAttack::BlinkAttack() : Blink(L"BlinkAttack")
{
}

BlinkAttack::~BlinkAttack()
{
}

void BlinkAttack::CheackCollision()
{
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

	FVector		location = owner->GetLocation();
	FRotator	rotation = owner->GetRotation();
	FVector		foward = rotation.GetForwardVector();
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

	Protocol::S2C_ReactionSkill reactionSkill;
	reactionSkill.set_remote_id(owner->GetGameObjectID());
	reactionSkill.set_object_id(this->GetGameObjectID());
	reactionSkill.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Blink_Attack));
	reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(location));
	reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(rotation));
	reactionSkill.set_duration(worldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
	this->BrodcastPlayerViewers(sendBuffer);
}

void BlinkAttack::OnParrying(ActorPtr inActor)
{
	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}

	if (false == this->CanParrying())
	{
		return;
	}

	bool ret = world->DestroyActor(this->GetGameObjectID());
	if (false == ret)
	{
		this->GameObjectLog(L"Can't destroy parrying\n");
	}
}

//==========================//
//         BlinkSturn		//
//==========================//

BlinkSturn::BlinkSturn() : Blink(L"BlinkSturn")
{
}

BlinkSturn::~BlinkSturn()
{
}

void BlinkSturn::CheackCollision()
{
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

	FVector		location = this->GetLocation();
	FRotator	rotation = this->GetRotation();
	FVector		foward = rotation.GetForwardVector();
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

		PlayerCharacterPtr player = std::static_pointer_cast<PlayerCharacter>(actor);
		if (nullptr == player)
		{
			continue;
		}

		StatsComponent& playerStats = player->GetStatComponent();
		BuffComponent& playerbuff = player->GetBuffComponent();

		float movement = playerStats.GetCurrentStats().GetMovementSpeed();

		//this->PushTask(worldTime, &BlinkSturn::SturnBeign, player, movement);
		//this->PushTask(worldTime + 3000, &BlinkSturn::SturnEnd, player, movement);
	}

	Protocol::S2C_ReactionSkill reactionSkill;
	reactionSkill.set_remote_id(owner->GetGameObjectID());
	reactionSkill.set_object_id(this->GetGameObjectID());
	reactionSkill.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Blink_Sturn));
	reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(location));
	reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(rotation));
	reactionSkill.set_duration(worldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
	this->BrodcastPlayerViewers(sendBuffer);
}

void BlinkSturn::OnParrying(ActorPtr inActor)
{
}

void BlinkSturn::SturnBeign(PlayerCharacterPtr inPlayer, float inMovementSpeed)
{
	if (inPlayer)
	{
		StatsComponent& playerStats = inPlayer->GetStatComponent();
		BuffComponent& playerbuff = inPlayer->GetBuffComponent();
		playerbuff.PushBuff(playerStats, EStatType::Stat_MovementSpeed, -inMovementSpeed);
	}
}

void BlinkSturn::SturnEnd(PlayerCharacterPtr inPlayer, float inMovementSpeed)
{
	if (inPlayer)
	{
		StatsComponent& playerStats = inPlayer->GetStatComponent();
		BuffComponent& playerbuff = inPlayer->GetBuffComponent();
		playerbuff.ReleaseBuff(playerStats, EStatType::Stat_MovementSpeed, -inMovementSpeed);
	}
}
