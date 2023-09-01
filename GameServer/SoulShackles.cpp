#include "pch.h"
#include "SoulShackles.h"

SoulShackles::SoulShackles() : EnemyAttack(L"SoulShackles"), mStartTime(0), mEndTime(10000)
{
	this->mDefaultCollisionComponent = new SphereCollisionComponent;
}

SoulShackles::~SoulShackles()
{
}

void SoulShackles::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	this->SetTick(true, SYSTEM_TICK);

	SphereCollisionComponent* collision = GetSphereCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetSphereCollisione(400.0f);

	this->SetDamage(1.0f);
	this->SetTargetActorType(EActorType::Player);
	this->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Place);

	Stats buff;
	buff.InitStats(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -100.0f, 0.0f);
	mStatsComponent.InitMaxStats(buff);

	world->PushTask(worldTime + 10000, &GameWorld::DestroyActor, this->GetGameObjectID());
}

void SoulShackles::OnDestroy()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const Stats& stat = this->mStatsComponent.GetMaxStats();

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
			playerbuff.ReleaseBuff(playerStats, EStatType::Stat_MovementSpeed, stat.GetMovementSpeed());
		}
	}

	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void SoulShackles::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	this->CheackCollision();

}

bool SoulShackles::IsValid()
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

void SoulShackles::OnAppearActor(ActorPtr inAppearActor)
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
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Soul_Shackles));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime - this->mStartTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void SoulShackles::OnDisAppearActor(ActorPtr inDisappearActor)
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

void SoulShackles::CheackCollision()
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

	const Stats&				stat = this->mStatsComponent.GetMaxStats();

	const float debugDuration = 0.1f;
	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), location, radius, debugDuration);

	uint8 findActorType = static_cast<uint8>(this->mTargetActorType);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(sphereTrace, findActorType, findActors);
	if (!result)
	{
		return;
	}

	for (auto player = mOverlapPlayer.begin(); player != mOverlapPlayer.end(); player++)
	{
		player->second = false;
	}

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
			mOverlapPlayer.insert(std::make_pair(remotePlayer->GetGameObjectID(), true));

			StatsComponent& playerStats = player->GetStatComponent();
			BuffComponent& playerbuff = player->GetBuffComponent();
			playerbuff.PushBuff(playerStats, EStatType::Stat_MovementSpeed, stat.GetMovementSpeed());
		}

	}

	for (auto player = mOverlapPlayer.begin(); player != mOverlapPlayer.end(); player++)
	{
		if (player->second == false)
		{

			GameRemotePlayerPtr overlapRemotePlayer = nullptr;
			if (true == world->IsValidPlayer(player->first, overlapRemotePlayer))
			{
				PlayerCharacterPtr playerCharacter = std::static_pointer_cast<PlayerCharacter>(overlapRemotePlayer->GetCharacter());
				if (nullptr == playerCharacter)
				{
					continue;
				}

				StatsComponent& playerStats = playerCharacter->GetStatComponent();
				BuffComponent& playerbuff = playerCharacter->GetBuffComponent();
				playerbuff.ReleaseBuff(playerStats, EStatType::Stat_MovementSpeed, stat.GetMovementSpeed());
			}

			mOverlapPlayer.erase(player++);
		}
		else
		{
			++player;
		}
	}

}

void SoulShackles::OnParrying(ActorPtr inActor)
{
}

SphereCollisionComponent* SoulShackles::GetSphereCollisionComponent()
{
	return static_cast<SphereCollisionComponent*>(this->GetDefaultCollisionComponent());
}
