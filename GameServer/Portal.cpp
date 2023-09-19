#include "pch.h"
#include "Portal.h"

Portal::Portal() : SphereTrigger(), mMaxNumber(0), mTeleportLocation(), mMaxTeleportTime(10000), mCurTeleportTime(0), mIsTeleport(false)
{
}

Portal::~Portal()
{
}

void Portal::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	this->SetTick(true, SYSTEM_TICK);

	SphereCollisionComponent* collision = this->GetSphereCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetSphereCollisione(300.0f);
}

void Portal::OnAppearActor(ActorPtr inAppearActor)
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

	Protocol::S2C_AppearProtal appearPacket;
	appearPacket.set_object_id(this->GetGameObjectID());
	appearPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));


	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearPacket);
	anotherPlayerState->Send(sendBuffer);
}

void Portal::OnDisAppearActor(ActorPtr inDisappearActor)
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

void Portal::OnBeginOverlap(ActorPtr inBeginOverlapActor)
{
	if (mMaxNumber == mOverlapActor.size() && false == mIsTeleport)
	{
		EnterTeleport();
	}
}

void Portal::OnEndOverlap(ActorPtr inEndOverlapActor)
{
	if (true == mIsTeleport)
	{
		EndTeleport();
	}
}

void Portal::OnOverlapTick(const int64 inDeltaTime)
{
	if (true == mIsTeleport)
	{
		mCurTeleportTime += inDeltaTime;
		if (mCurTeleportTime >= mMaxTeleportTime)
		{
			this->Teleport();
		}
	}
}

void Portal::EnterTeleport()
{
	mIsTeleport = true;
}

void Portal::EndTeleport()
{
	mIsTeleport = false;
	mCurTeleportTime = 0;
}

void Portal::Teleport()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	for (auto overlap : mOverlapActor)
	{
		PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(overlap.first);
		if (nullptr == character)
		{
			return;
		}

		character->GetMovementComponent().SetNewDestination(character, mTeleportLocation, mTeleportLocation, worldTime, 0.0f);

		Protocol::S2C_Teleport teleportPacket;
		teleportPacket.set_object_id(character->GetGameObjectID());
		teleportPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(mTeleportLocation));

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, teleportPacket);
		this->BrodcastPlayerViewers(sendBuffer);
	}

	mIsTeleport	= false;
	mCurTeleportTime = 0;
	mOverlapActor.clear();
}

void Portal::SetMaxNumber(int32 inMaxNumber)
{
	mMaxNumber = inMaxNumber;
}

void Portal::SetTeleportLocation(const FVector& inLocation)
{
	mTeleportLocation = inLocation;
}