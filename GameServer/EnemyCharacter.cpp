#include "pch.h"
#include "EnemyCharacter.h"

EnemyCharacter::EnemyCharacter(const WCHAR* inName) : Pawn(inName), mEnemyID(0), mSpawnObjectID(0), mStateManager(), mSpawnLocation()
{
}

EnemyCharacter::~EnemyCharacter()
{
}

void EnemyCharacter::OnInitialization()
{
	SetTick(DEFAULT_SYNC_TICK, false);
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

void EnemyCharacter::AppearActor(PlayerStatePtr inClosePlayerState)
{
	RemotePlayerPtr targetRemotePlayer = inClosePlayerState->GetRemotePlayer();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	if (false == IsValid())
	{
		return;
	}

	Viewers& viewers = this->GetViewers();
	if (viewers.find(inClosePlayerState) != viewers.end())
	{
		return;
	}
	viewers.insert(inClosePlayerState);
	inClosePlayerState->GetMonitorActors().insert(GetActorPtr());

	Protocol::S2C_AppearEnemy appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.set_enemy_id(this->GetEnemyID());
	appearPacket.mutable_enemy()->CopyFrom(this->ConvertSEnemy());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inClosePlayerState);
	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, appearPacket);
	inClosePlayerState->Send(appearItemSendBuffer);
}

void EnemyCharacter::DisAppearActor(PlayerStatePtr inClosePlayerState)
{
	RemotePlayerPtr targetRemotePlayer = inClosePlayerState->GetRemotePlayer();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	Viewers& viewers = this->GetViewers();
	if (viewers.find(inClosePlayerState) == viewers.end())
	{
		return;
	}
	this->GetViewers().erase(inClosePlayerState);
	inClosePlayerState->GetMonitorActors().erase(this->GetActorPtr());

	Protocol::S2C_DisAppearGameObject disAppearItemPacket;
	disAppearItemPacket.set_object_id(this->GetGameObjectID());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inClosePlayerState);
	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disAppearItemPacket);
	inClosePlayerState->Send(appearItemSendBuffer);
}

bool EnemyCharacter::IsAttackRange()
{
	if (false == IsValid())
	{
		return false;
	}

	mStats.GetRange();

	return false;
}

void EnemyCharacter::SetEnemeyID(const int32 inEnemyID)
{
	mEnemyID = inEnemyID;
}

void EnemyCharacter::SetSpawnObjectID(const int64 inSpawnObjectID)
{
	mSpawnObjectID = inSpawnObjectID;
}

void EnemyCharacter::SetSpawnLocation(const Protocol::SVector& inSpawnLocation)
{
	mSpawnLocation = inSpawnLocation;
}

void EnemyCharacter::SetAggroPlayer(const CharacterPtr& inCharacter)
{
	mAggroPlayer = inCharacter;
}

const Protocol::SEnemy EnemyCharacter::ConvertSEnemy()
{
	Protocol::SEnemy tempEnemy;
	tempEnemy.set_state(static_cast<Protocol::EEnemyState>(this->GetStateManager().GetCurrentStateType()));
	tempEnemy.set_hp(this->GetStats().GetHealth());
	tempEnemy.mutable_location()->CopyFrom(GetLocation());
	tempEnemy.mutable_rotation()->CopyFrom(this->GetRotation());
	return tempEnemy;
}
