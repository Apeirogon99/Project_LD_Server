#include "pch.h"
#include "Portal.h"

Portal::Portal() : SphereTrigger(), mTeleportLocation(), mMaxTeleportTime(10000), mCurTeleportTime(0), mLastTeleportTime(10), mIsTeleport(false)
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
	if (mMaxOverlap == mOverlapActor.size() && false == mIsTeleport)
	{
		EnterTeleport();
	}
}

void Portal::OnEndOverlap(ActorPtr inEndOverlapActor)
{
	if (true == mIsTeleport)
	{
		LeaveTeleport();
	}
}

void Portal::OnOverlapTick(const int64 inDeltaTime)
{
	if (true == mIsTeleport)
	{
		mCurTeleportTime += inDeltaTime;
		if (mCurTeleportTime >= mMaxTeleportTime)
		{
			this->BeginTeleport();
		}
		else
		{
			this->EnterTeleport();
		}
	}
}

void Portal::EnterTeleport()
{
	mIsTeleport = true;

	float time = (mMaxTeleportTime - mCurTeleportTime) / 1000.0f;
	int64 iTime = static_cast<int64>(time + 1);
	std::string title = "잠시후 다음 지점으로 이동됩니다.";

	if (mLastTeleportTime < iTime)
	{
		return;
	}
	mLastTeleportTime = iTime - 1;

	Protocol::S2C_EnterPortal enterPortalPacket;
	enterPortalPacket.set_title(title);
	enterPortalPacket.set_time(iTime);

	this->GameObjectLog(L"Waiting for teleport %lld\n", iTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, enterPortalPacket);
	this->BroadCastOverlap(sendBuffer);

}

void Portal::LeaveTeleport()
{
	mIsTeleport = false;
	mCurTeleportTime = 0;
	mLastTeleportTime = static_cast<int64>(mMaxTeleportTime / 1000.0f);

	this->GameObjectLog(L"Leave teleport\n");

	Protocol::S2C_LeavePortal leavePortalPacket;

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePortalPacket);
	this->BroadCastOverlap(sendBuffer);
}

void Portal::BeginTeleport()
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

		FVector newTeleportLocation = Random::GetRandomVectorInRange2D(mTeleportLocation, 300.0f);

		Protocol::S2C_Teleport teleportPacket;
		teleportPacket.set_object_id(character->GetGameObjectID());
		teleportPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(newTeleportLocation));

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, teleportPacket);
		this->BroadCastOverlap(sendBuffer);

		character->PushTask(worldTime, &PlayerCharacter::Teleport, newTeleportLocation);
	}

	//this->PushTask(worldTime + 1000, &Portal::EndTeleport);

	LeaveTeleport();
	mOverlapActor.clear();
}

void Portal::EndTeleport()
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
		character->PushTask(worldTime, &PlayerCharacter::Teleport, mTeleportLocation);

	}

	LeaveTeleport();
	mOverlapActor.clear();

}

void Portal::SetWaitTeleprotTime(int64 inMaxTeleportTime)
{
	mMaxTeleportTime = inMaxTeleportTime;
}

void Portal::SetTeleportLocation(const FVector& inLocation)
{
	mTeleportLocation = inLocation;
}

void Portal::BroadCastOverlap(SendBufferPtr inSendBuffer)
{
	for (auto overlap : mOverlapActor)
	{
		PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(overlap.first);
		if (nullptr == character)
		{
			return;
		}

		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(character->GetOwner().lock());
		if (nullptr == remotePlayer)
		{
			return;
		}

		PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(remotePlayer->GetRemoteClient().lock());
		if (nullptr == playerState)
		{
			return;
		}

		playerState->Send(inSendBuffer);
	}
}
