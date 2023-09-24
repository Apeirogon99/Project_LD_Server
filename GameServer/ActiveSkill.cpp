#include "pch.h"
#include "ActiveSkill.h"

ActiveSkill::ActiveSkill(const WCHAR* inName) : Actor(inName), mSKillID(0), mActiveTime(0), mDeActiveTime(0)
{
}

ActiveSkill::~ActiveSkill()
{
}

void ActiveSkill::OnAppearActor(ActorPtr inAppearActor)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	GameRemotePlayerPtr ownerRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == ownerRemotePlayer)
	{
		return;
	}

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

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(ownerRemotePlayer->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(this->mSKillID);
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime - this->mActiveTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void ActiveSkill::OnDisAppearActor(ActorPtr inDisappearActor)
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

void ActiveSkill::BeginCastingSkill()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return;
	}

	character->GetMovementComponent().SetRestrictMovement(true);

	FVector destinationLocation = this->GetLocation() + this->GetRotation().GetForwardVector() * 1.0f;
	character->GetMovementComponent().SetNewDestination(this->GetActorPtr(), character->GetLocation(), destinationLocation, worldTime, 0.0f);
}

void ActiveSkill::EndCastingSkill()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64 worldTime = world->GetWorldTime();

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int64& remoteID = remotePlayer->GetGameObjectID();

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return;
	}
	character->GetMovementComponent().SetRestrictMovement(false);

	Protocol::S2C_EndReactionSkill endSkillPacket;
	endSkillPacket.set_remote_id(remoteID);
	endSkillPacket.set_timestamp(worldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, endSkillPacket);
	remotePlayer->BrodcastPlayerViewers(sendBuffer);
}

void ActiveSkill::SetActiveSkill(const int32& inSkillID, const int64& inActiveTime)
{
	mSKillID = inSkillID;
	mActiveTime = inActiveTime;
}

void ActiveSkill::Active()
{
}

void ActiveSkill::DeActive(const int64& inDuration)
{

	if (false == IsValid())
	{
		return;
	}


	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetNextWorldTime();

	mDeActiveTime = worldTime + inDuration;
	world->PushTask(worldTime + inDuration, &GameWorld::DestroyActor, this->GetGameObjectID());

}

const int32& ActiveSkill::GetSkillID() const
{
	return mSKillID;
}
