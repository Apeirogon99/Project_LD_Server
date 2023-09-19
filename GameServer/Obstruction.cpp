#include "pch.h"
#include "Obstruction.h"

Obstruction::Obstruction() : Actor(L"Obstruction"), mObstructionType(0)
{
    this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

Obstruction::~Obstruction()
{
}

void Obstruction::OnInitialization()
{
    this->SetActorType(static_cast<int8>(EActorType::Obstruction));
}

void Obstruction::OnDestroy()
{
}

void Obstruction::OnTick(const int64 inDeltaTime)
{
}

bool Obstruction::IsValid()
{
    return true;
}

void Obstruction::OnAppearActor(ActorPtr inAppearActor)
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

	Protocol::S2C_AppearObstruction appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.set_type(this->mObstructionType);
	appearPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearPacket.mutable_extent()->CopyFrom(PacketUtils::ToSVector(this->GetBoxCollisionComponent()->GetBoxCollision().GetBoxExtent()));

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearPacket);
	anotherPlayerState->Send(appearItemSendBuffer);
}

void Obstruction::OnDisAppearActor(ActorPtr inDisAppearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inDisAppearActor);
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

	Protocol::S2C_DisAppearGameObject disappearPacket;
	disappearPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearPacket);
	anotherPlayerState->Send(appearItemSendBuffer);
}

void Obstruction::SetObstructionName(const WCHAR* inName)
{
	this->SetGameObjectName(inName);
}

void Obstruction::SetObstructionType(const int32& inType)
{
	mObstructionType = inType;
}

void Obstruction::SetBoxCollisionExtent(FVector inExtent)
{
    BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
    collision->SetOwner(this->GetActorRef());
    collision->SetBoxCollision(inExtent);
}

BoxCollisionComponent* Obstruction::GetBoxCollisionComponent() const
{
    return static_cast<BoxCollisionComponent*>(this->mDefaultCollisionComponent);
}
