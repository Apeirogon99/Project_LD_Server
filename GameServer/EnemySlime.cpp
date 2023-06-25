#include "pch.h"
#include "EnemySlime.h"

EnemySlime::EnemySlime() : EnemyCharacter(L"[Enemy::Slime]")
{
}

EnemySlime::~EnemySlime()
{
}

void EnemySlime::Initialization()
{
}

void EnemySlime::Destroy()
{
	std::shared_ptr<EnemySpawner<EnemySlime>> spawner = std::static_pointer_cast<EnemySpawner<EnemySlime>>(GetOwner().lock());
	if (nullptr == spawner)
	{
		return;
	}

	spawner->OnDestroyEnemy(GetGameObjectID());
}

void EnemySlime::Tick()
{
}

bool EnemySlime::IsValid()
{
	return GetEnemyID() != 0;
}

void EnemySlime::AppearActor(PlayerStatePtr inClosePlayerState)
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

	ViewActors& viewActors = targetRemotePlayer->GetViewActors();
	if (viewActors.find(GetGameObjectPtr()) != viewActors.end())
	{
		return;
	}
	targetRemotePlayer->GetViewActors().insert(GetGameObjectPtr());

	Protocol::S2C_AppearEnemy appearPacket;
	appearPacket.mutable_enemy()->CopyFrom(this->ConvertSEnemy());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inClosePlayerState);
	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, appearPacket);
	inClosePlayerState->Send(appearItemSendBuffer);
}

void EnemySlime::DisAppearActor(PlayerStatePtr inClosePlayerState)
{
	RemotePlayerPtr targetRemotePlayer = inClosePlayerState->GetRemotePlayer();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	ViewActors& viewActors = targetRemotePlayer->GetViewActors();
	if (viewActors.find(GetGameObjectPtr()) == viewActors.end())
	{
		return;
	}
	targetRemotePlayer->GetViewActors().erase(GetGameObjectPtr());

	Protocol::S2C_DisAppearGameObject disAppearItemPacket;
	disAppearItemPacket.set_object_id(this->GetGameObjectID());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inClosePlayerState);
	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disAppearItemPacket);
	inClosePlayerState->Send(appearItemSendBuffer);
}

void EnemySlime::Attack()
{
}

void EnemySlime::Hit()
{
}

void EnemySlime::Death()
{
}
