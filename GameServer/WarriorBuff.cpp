#include "pch.h"
#include "WarriorBuff.h"

WarriorBuff::WarriorBuff() : ActiveSkill(L"WarriorBuff")
{
}

WarriorBuff::~WarriorBuff()
{
}

void WarriorBuff::OnInitialization()
{
	this->SetTick(true, SYSTEM_TICK);

	mSphereCollisionComponent.SetSphereCollisione(400.0f);
	mSphereCollisionComponent.SetOwner(this->GetActorRef());

	Stats buff;
	buff.InitStats(0.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 50.0f, 0.0f);
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
			BuffComponent& playerbuff = playerCharacter->GetBuffComponent();
			playerbuff.ReleaseBuff(playerStats, EStatType::Stat_AttackDamage, stat.GetAttackDamage());
			playerbuff.ReleaseBuff(playerStats, EStatType::Stat_Armor, stat.GetArmor());
			playerbuff.ReleaseBuff(playerStats, EStatType::Stat_MovementSpeed, stat.GetMovementSpeed());
		}
	}

	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void WarriorBuff::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	this->Active();
}

bool WarriorBuff::IsValid()
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

void WarriorBuff::Active()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime	= world->GetWorldTime();

	GameRemotePlayerPtr owner = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	PartyPtr party = owner->GetParty();
	if (nullptr == party)
	{
		return;
	}

	FVector		location	= this->GetLocation();
	const float radius		= this->mSphereCollisionComponent.GetSphereCollision().GetRadius();
	const Stats& stat		= this->mStatsComponent.GetMaxStats();

	//DEBUG
	const float duration = (worldTime - this->mDeActiveTime) / 1000.0f;
	PacketUtils::DebugDrawSphere(owner->GetRemoteClient().lock(), location, radius, duration);

	uint8 findActorType = static_cast<uint8>(EActorType::Player);
	std::vector<ActorPtr> findActors;
	bool result = world->FindActors(location, radius, findActorType, findActors);

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
				if (owner->GetGameObjectID() == playerGameObjectID)
				{
					mOverlapPlayer.insert(std::make_pair(playerGameObjectID, true));

					StatsComponent& playerStats = overlapPlayer->GetStatComponent();
					BuffComponent& playerbuff = overlapPlayer->GetBuffComponent();
					playerbuff.PushBuff(playerStats, EStatType::Stat_AttackDamage, stat.GetAttackDamage());
					playerbuff.PushBuff(playerStats, EStatType::Stat_Armor, stat.GetArmor());
					playerbuff.PushBuff(playerStats, EStatType::Stat_MovementSpeed, stat.GetMovementSpeed());
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
				BuffComponent& playerbuff = overlapPlayer->GetBuffComponent();
				playerbuff.PushBuff(playerStats, EStatType::Stat_AttackDamage, stat.GetAttackDamage());
				playerbuff.PushBuff(playerStats, EStatType::Stat_Armor, stat.GetArmor());
				playerbuff.PushBuff(playerStats, EStatType::Stat_MovementSpeed, stat.GetMovementSpeed());
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

				StatsComponent& playerStats = playerCharacter->GetStatComponent();
				BuffComponent& playerbuff = playerCharacter->GetBuffComponent();
				playerbuff.ReleaseBuff(playerStats, EStatType::Stat_AttackDamage, stat.GetAttackDamage());
				playerbuff.ReleaseBuff(playerStats, EStatType::Stat_Armor, stat.GetArmor());
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