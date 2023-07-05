#include "pch.h"
#include "EnemyCharacter.h"

EnemyCharacter::EnemyCharacter(const WCHAR* inName) : Character(inName), mEnemyID(0), mSpawnObjectID(0), mStateManager()
{
}

EnemyCharacter::~EnemyCharacter()
{
}

void EnemyCharacter::OnInitialization()
{
	SetTick(false, DEFAULT_SYNC_TICK);
}

void EnemyCharacter::OnDestroy()
{
}

void EnemyCharacter::OnTick(const int64 inDeltaTime)
{
	
}

bool EnemyCharacter::IsValid()
{
	return GetEnemyID() != 0;
}

void EnemyCharacter::OnAppearActor(ActorPtr inAppearActor)
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

	Protocol::S2C_AppearEnemy appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.set_enemy_id(this->GetEnemyID());
	appearPacket.mutable_enemy()->CopyFrom(this->ConvertSEnemy());

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearPacket);
	anotherPlayerState->Send(appearItemSendBuffer);
}

void EnemyCharacter::OnDisAppearActor(ActorPtr inDisappearActor)
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

	if (mPlayerViewers.find(anotherPlayerState) == mPlayerViewers.end())
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

void EnemyCharacter::SetEnemeyID(const int32 inEnemyID)
{
	mEnemyID = inEnemyID;
}

void EnemyCharacter::SetSpawnObjectID(const int64 inSpawnObjectID)
{
	mSpawnObjectID = inSpawnObjectID;
}

void EnemyCharacter::SetRecoveryLocation(const Location& inRecoveryLocation)
{
	mRecoveryLocation = inRecoveryLocation;
}

void EnemyCharacter::SetEnemyStats(const Stats& inEnemyStats)
{
	mStatsComponent.InitMaxStats(inEnemyStats);
}

void EnemyCharacter::SetAggroPlayer(const CharacterPtr& inCharacter)
{
	mAggroPlayer = inCharacter;
}

const Protocol::SEnemy EnemyCharacter::ConvertSEnemy()
{
	Protocol::SEnemy tempEnemy;
	tempEnemy.set_state(static_cast<Protocol::EEnemyState>(this->mStateManager.GetCurrentStateType()));
	tempEnemy.set_hp(this->mStatsComponent.GetCurrentStats().GetHealth());
	tempEnemy.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	tempEnemy.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	return tempEnemy;
}
