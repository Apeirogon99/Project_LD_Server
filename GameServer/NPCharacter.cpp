#include "pch.h"
#include "NPCharacter.h"

NPCharacter::NPCharacter(const WCHAR* inName) : Character(L"NPCharacter")
{
}

NPCharacter::~NPCharacter()
{
}

void NPCharacter::OnInitialization()
{
}

void NPCharacter::OnDestroy()
{
}

void NPCharacter::OnTick(const int64 inDeltaTime)
{
}

bool NPCharacter::IsValid()
{
	return true;
}

void NPCharacter::OnAppearActor(ActorPtr inAppearActor)
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

	FVector		location = this->GetLocation();
	FRotator	rotation = this->GetRotation();
	const int64	worldTime = this->GetWorld().lock()->GetWorldTime();

	//Protocol::S2C_AppearNPC appearPacket;
	//appearPacket.set_object_id(this->GetGameObjectID());
	//appearPacket.set_npc_id(this->mNPCID);
	//appearPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(location));
	//appearPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSVector(rotation));
	//appearPacket.set_timestamp(worldTime);

	//SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearPacket);
	//anotherPlayerState->Send(sendBuffer);
}

void NPCharacter::OnDisAppearActor(ActorPtr inDisappearActor)
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

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disAppearItemPacket);
	anotherPlayerState->Send(sendBuffer);
}
