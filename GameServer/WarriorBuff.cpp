#include "pch.h"
#include "WarriorBuff.h"

WarriorBuff::WarriorBuff() : Actor(L"WarriorBuff"), mDuration(0)
{
}

WarriorBuff::~WarriorBuff()
{
}

void WarriorBuff::OnInitialization()
{
	this->SetTick(true, SYSTEM_TICK);

	mSphereCollisionComponent.SetSphereCollisione(100.0f);
	mSphereCollisionComponent.SetOwner(this->GetActorRef());

	Stats buff;
	buff.InitStats(0.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 50.0f, 0.0f); //공격력, 아머, 이동속도 증가
	mStatsComponent.InitMaxStats(buff);
}

void WarriorBuff::OnDestroy()
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
			playerStats.UpdateCurrentStat(EStatType::Stat_AttackDamage, playerStats.GetCurrentStats().GetAttackDamage() - stat.GetAttackDamage());
			playerStats.UpdateCurrentStat(EStatType::Stat_Armor, playerStats.GetCurrentStats().GetArmor() - stat.GetArmor());
			playerStats.UpdateCurrentStat(EStatType::Stat_MovementSpeed, playerStats.GetCurrentStats().GetMovementSpeed() - stat.GetMovementSpeed());
		}
	}

	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void WarriorBuff::OnTick(const int64 inDeltaTime)
{
	if (!IsValid())
	{
		return;
	}

	this->CheackCollision();

	mDuration -= inDeltaTime;
	if (mDuration <= 0)
	{
		GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
		if (nullptr == world)
		{
			return;
		}

		bool ret = world->DestroyActor(this->GetGameObjectID());
		if (false == ret)
		{
			this->mDuration = 0;
			this->GameObjectLog(L"Can't destroy buff\n");
		}

	}
}

bool WarriorBuff::IsValid()
{
	return mDuration > 0;
}

void WarriorBuff::OnAppearActor(ActorPtr inAppearActor)
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

	if (true == this->FindPlayerViewer(anotherPlayerState))
	{
		return;
	}
	this->InsertPlayerViewer(anotherPlayerState);
	anotherPlayerState->InsertActorMonitor(this->GetActorPtr());

	Protocol::S2C_AppearBuff appearBuffPacket;
	appearBuffPacket.set_remote_id(this->GetGameObjectID());
	appearBuffPacket.set_object_id(this->GetGameObjectID());
	appearBuffPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearBuffPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearBuffPacket.set_duration(this->GetDuration());

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearBuffPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void WarriorBuff::OnDisAppearActor(ActorPtr inDisappearActor)
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

	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr disappearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	anotherPlayerState->Send(disappearSendBuffer);
}

void WarriorBuff::CheackCollision()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime	= world->GetWorldTime();

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		bool ret = world->DestroyActor(this->GetGameObjectID());
		if (false == ret)
		{
			this->mDuration = 0;
			this->GameObjectLog(L"Can't destroy buff\n");
		}
		return;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return;
	}

	FVector		location	= this->GetLocation();
	const float radius		= this->mSphereCollisionComponent.GetSphereCollision().GetRadius();
	const Stats& stat		= this->mStatsComponent.GetMaxStats();

	uint8 findActorType = static_cast<uint8>(EActorType::Player);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(location, radius, findActorType, findActors);
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
		PlayerCharacterPtr overlapPlayer = std::static_pointer_cast<PlayerCharacter>(actor);
		if (nullptr == overlapPlayer)
		{
			continue;
		}

		GameRemotePlayerPtr overlapRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(overlapPlayer->GetOwner().lock());
		if (nullptr == overlapRemotePlayer)
		{

		}
		const int64& playerGameObjectID = overlapRemotePlayer->GetGameObjectID();

		if (false == party->IsParty())
		{
			auto findOverlapPlayer = mOverlapPlayer.find(playerGameObjectID);
			if (findOverlapPlayer == mOverlapPlayer.end())
			{
				if (remotePlayer->GetGameObjectID() == playerGameObjectID)
				{
					mOverlapPlayer.insert(std::make_pair(playerGameObjectID, true));

					StatsComponent& playerStats = overlapPlayer->GetStatComponent();
					playerStats.UpdateCurrentStat(EStatType::Stat_AttackDamage, playerStats.GetCurrentStats().GetAttackDamage() + stat.GetAttackDamage());
					playerStats.UpdateCurrentStat(EStatType::Stat_Armor, playerStats.GetCurrentStats().GetArmor() + stat.GetArmor());
					playerStats.UpdateCurrentStat(EStatType::Stat_MovementSpeed, playerStats.GetCurrentStats().GetMovementSpeed() + stat.GetMovementSpeed());
				}
			}
			else
			{
				findOverlapPlayer->second = true;
			}

		}
		else
		{
			const std::map<int64, PlayerCharacterRef> partyPlayers = party->GetPartyPlayers();
			auto findParty = partyPlayers.find(playerGameObjectID);
			if (findParty == partyPlayers.end())
			{
				continue;
			}

			auto findOverlapPlayer = mOverlapPlayer.find(playerGameObjectID);
			if (findOverlapPlayer == mOverlapPlayer.end())
			{
				mOverlapPlayer.insert(std::make_pair(playerGameObjectID, true));

				StatsComponent& playerStats = overlapPlayer->GetStatComponent();
				playerStats.UpdateCurrentStat(EStatType::Stat_AttackDamage, playerStats.GetCurrentStats().GetAttackDamage() + stat.GetAttackDamage());
				playerStats.UpdateCurrentStat(EStatType::Stat_Armor, playerStats.GetCurrentStats().GetArmor() + stat.GetArmor());
				playerStats.UpdateCurrentStat(EStatType::Stat_MovementSpeed, playerStats.GetCurrentStats().GetMovementSpeed() + stat.GetMovementSpeed());
			}
			else
			{
				findOverlapPlayer->second = true;
			}
		}
	}

	for (auto player = mOverlapPlayer.begin(); player != mOverlapPlayer.end();)
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
				const int64& playerGameObjectID = playerCharacter->GetGameObjectID();

				StatsComponent& playerStats = playerCharacter->GetStatComponent();
				playerStats.UpdateCurrentStat(EStatType::Stat_AttackDamage, playerStats.GetCurrentStats().GetAttackDamage() - stat.GetAttackDamage());
				playerStats.UpdateCurrentStat(EStatType::Stat_Armor, playerStats.GetCurrentStats().GetArmor() - stat.GetArmor());
				playerStats.UpdateCurrentStat(EStatType::Stat_MovementSpeed, playerStats.GetCurrentStats().GetMovementSpeed() - stat.GetMovementSpeed());
			}

			mOverlapPlayer.erase(player++);
		}

		++player;
	}
}

void WarriorBuff::SetDuration(const int64& inDuration)
{
	mDuration = inDuration;
}

const int64& WarriorBuff::GetDuration()
{
	return mDuration;
}
