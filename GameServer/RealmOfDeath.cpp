#include "pch.h"
#include "RealmOfDeath.h"

RealmOfDeath::RealmOfDeath() : EnemyAttack(L"RealmOfDeath"), mStartTime(0), mEndTime(10000), mActive(false)
{
	this->mDefaultCollisionComponent = new SphereCollisionComponent;
}

RealmOfDeath::~RealmOfDeath()
{
}

void RealmOfDeath::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetNextWorldTime();

	this->SetTick(true, SECOND_TICK);

	SphereCollisionComponent* collision = GetSphereCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetSphereCollisione(2000.0f);

	this->SetDamage(1.0f);
	this->SetActorType(static_cast<uint8>(EActorType::EnemyAttack));
	this->SetTargetActorType(EActorType::Player);
	this->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Place);

	Stats buff;
	buff.InitStats(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f);
	mStatsComponent.InitMaxStats(buff);

	this->PushTask(worldTime, &RealmOfDeath::CheackTargeting);
}

void RealmOfDeath::OnDestroy()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	for (auto player = mOverlapPlayer.begin(); player != mOverlapPlayer.end(); player++)
	{
		GameRemotePlayerPtr overlapRemotePlayer = nullptr;
		if (true == world->IsValidPlayer(player->first, overlapRemotePlayer))
		{
			PlayerCharacterPtr playerCharacter = std::static_pointer_cast<PlayerCharacter>(overlapRemotePlayer->GetCharacter());
			if (nullptr == playerCharacter)
			{
				continue;
			}
			const int64& playerGameObjectID = playerCharacter->GetGameObjectID();

			StatsComponent& playerStats = playerCharacter->GetStatComponent();
			BuffComponent& playerbuff = playerCharacter->GetBuffComponent();
			playerbuff.ReleaseBuff(playerStats, EBuffType::DeBuff_RealmOfDeath, EStatType::Stat_MovementSpeed, player->second);
		}
	}

	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void RealmOfDeath::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	if (this->mActive)
	{
		this->CheackCollision();
	}
}

bool RealmOfDeath::IsValid()
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

		world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
		return false;
	}

	return true;
}

void RealmOfDeath::OnAppearActor(ActorPtr inAppearActor)
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
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Realm_Of_Death));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime - this->mStartTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void RealmOfDeath::OnDisAppearActor(ActorPtr inDisappearActor)
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

void RealmOfDeath::CheackTargeting()
{
	this->mActive = true;

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();
	const int64& duration = mStartTime + 1000 - worldTime;

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	Location location = this->GetLocation();
	Rotation rotation = this->GetRotation();

	const float debugDuration = 1.0f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), location, 2000.0f, debugDuration);

	Protocol::S2C_ReactionSkill reactionSkill;
	reactionSkill.set_remote_id(owner->GetGameObjectID());
	reactionSkill.set_object_id(this->GetGameObjectID());
	reactionSkill.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Realm_Of_Death));
	reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(location));
	reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(rotation));
	reactionSkill.set_duration(duration);
}

void RealmOfDeath::CheackCollision()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	SphereCollisionComponent*	collision = GetSphereCollisionComponent();
	FVector						location = this->GetLocation();
	const float					radius = collision->GetSphereCollision().GetRadius();
	SphereTrace					sphereTrace(this->GetActorRef(), location, true, radius);

	const Stats& stat = this->mStatsComponent.GetMaxStats();

	uint8 findActorType = static_cast<uint8>(this->mTargetActorType);
	std::vector<ActorPtr> findActors;
	world->FindActors(sphereTrace, findActorType, findActors);

	for (ActorPtr actor : findActors)
	{
		PlayerCharacterPtr player = std::static_pointer_cast<PlayerCharacter>(actor);
		if (nullptr == player)
		{
			return;
		}

		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(player->GetOwner().lock());
		if (nullptr == remotePlayer)
		{
			return;
		}

		auto findPlayer = mOverlapPlayer.find(remotePlayer->GetGameObjectID());
		if (findPlayer == mOverlapPlayer.end())
		{
			mOverlapPlayer.insert(std::make_pair(remotePlayer->GetGameObjectID(), stat.GetMovementSpeed()));
		}
		else
		{
			float buff = findPlayer->second;
			findPlayer->second = buff + stat.GetMovementSpeed();
		}

		StatsComponent& playerStats = player->GetStatComponent();
		BuffComponent& playerbuff = player->GetBuffComponent();
		playerbuff.PushBuff(playerStats, EBuffType::DeBuff_RealmOfDeath, EStatType::Stat_MovementSpeed, stat.GetMovementSpeed());
	}
}

bool RealmOfDeath::OnParrying(ActorPtr inActor)
{
	return false;
}

SphereCollisionComponent* RealmOfDeath::GetSphereCollisionComponent()
{
	return static_cast<SphereCollisionComponent*>(this->GetDefaultCollisionComponent());
}
