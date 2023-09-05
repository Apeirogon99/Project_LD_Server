#include "pch.h"
#include "Rise.h"
#include "LichLifeVessel.h"

Rise::Rise() : EnemyAttack(L"Rise"), mSkillID(0)
{
	this->mDefaultCollisionComponent = new SphereCollisionComponent;
}

Rise::~Rise()
{
}


void Rise::CheackCollision()
{
}

void Rise::OnParrying(ActorPtr inActor)
{
}

void Rise::SetSpawnInfo(int32 inEnemyID, int32 inSKillID)
{
	mEnemyID = inEnemyID;
	mSkillID = inSKillID;
}

void Rise::SpawnEnemy()
{

	WorldPtr world = GetWorld().lock();
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	GameDatasPtr datas = std::static_pointer_cast<GameDatas>(world->GetDatas());
	if (nullptr == datas)
	{
		return;
	}
	const Stats& stat = datas->GetEnemyStat(this->mEnemyID);

	const Location		newLocation = this->GetLocation();
	const Rotation		newRoation = this->GetRotation();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	EnemyCharacterPtr newEnemy = nullptr;
	switch (static_cast<EnemyID>(mEnemyID))
	{
	case EnemyID::Enemy_Slime:
		newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<EnemySlime>(owner->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f)));
		break;
	case EnemyID::Enemy_Nomal_Skeleton:
		newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<EnemyNomalSkeleton>(owner->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f)));
		break;
	case EnemyID::Enemy_Warrior_Skeleton:
		newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<EnemyWarriorSkeleton>(owner->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f)));
		break;
	case EnemyID::Enemy_Archer_Skeleton:
		newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<EnemyArcherSkeleton>(owner->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f)));
		break;
	case EnemyID::Enemy_Dark_Skeleton:
		newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<EnemyNomalSkeleton>(owner->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f)));
		break;
	case EnemyID::Enemy_Dark_Knight:
		newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<EnemyDarkKnight>(owner->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f)));
		break;
	case EnemyID::Enemy_Lich_Life_Vessle:
		newEnemy = std::static_pointer_cast<EnemyCharacter>(world->SpawnActor<LichLifeVessel>(owner->GetGameObjectRef(), newLocation, newRoation, Scale(1.0f, 1.0f, 1.0f)));
		break;
	default:
		break;
	}
	if (nullptr == newEnemy)
	{
		return;
	}

	newEnemy->SetEnemeyID(this->mEnemyID);
	newEnemy->SetActorType(static_cast<uint8>(EActorType::Enemy));
	newEnemy->SetSpawnObjectID(this->GetGameObjectID());
	newEnemy->SetEnemyStats(stat);
	newEnemy->SetAggressive(true);
	newEnemy->SetRecoveryLocation(newLocation);
	newEnemy->GetStateManager().SetState(EStateType::State_Search);
	newEnemy->SetReward(false);

	bool ret = world->DestroyActor(this->GetGameObjectID());
	if (false == ret)
	{
		this->GameObjectLog(L"Can't destroy Skill\n");
	}
}

SphereCollisionComponent* Rise::GetSphereCollisionComponent()
{
	return static_cast<SphereCollisionComponent*>(this->GetDefaultCollisionComponent());
}

void Rise::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}
	mStartTime = world->GetWorldTime();

	SphereCollisionComponent* collision = GetSphereCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetSphereCollisione(50.0f);

	this->SetDamage(1.0f);
	this->SetTargetActorType(EActorType::Player);
	this->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Hard_Place);
}

void Rise::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void Rise::OnTick(const int64 inDeltaTime)
{
}

bool Rise::IsValid()
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

	if (this->mSkillID == 0)
	{
		return false;
	}

	return true;
}

void Rise::OnAppearActor(ActorPtr inAppearActor)
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
	appearSkillPacket.set_skill_id(this->mSkillID);
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime - this->mStartTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void Rise::OnDisAppearActor(ActorPtr inDisappearActor)
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
