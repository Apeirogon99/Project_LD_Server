#include "pch.h"
#include "Portal.h"

Portal::Portal() : Actor(L"Portal"), mCoolTime(0), mUse(false)
{
	this->mDefaultCollisionComponent = new BoxCollisionComponent;
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

	this->SetTick(true, GAME_TICK);

	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(100.0f, 100.0f, 100.0f));
}

void Portal::OnDestroy()
{
}

void Portal::OnTick(const int64 inDeltaTime)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (true == mUse)
	{
		mCoolTime += inDeltaTime;
		if (mCoolTime < 1000)
		{
			return;
		}
		mCoolTime = 0;
		mUse = false;
	}

	std::vector<ActorPtr> actors;
	bool result = world->FindActors(this->GetLocation(), 100.0f, static_cast<uint8>(EActorType::Player), actors, 1);
	if (result)
	{
		this->OnInteractive(actors.at(0));
	}
}

bool Portal::IsValid()
{
	return true;
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

void Portal::OnInteractive(ActorPtr inActor)
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	const float distance = FVector::Distance(this->GetLocation(), inActor->GetLocation());
	if (100.0f < distance)
	{
		return;
	}

	PlayerCharacterPtr player = std::static_pointer_cast<PlayerCharacter>(inActor);
	if (nullptr == player)
	{
		return;
	}

	//player->SetLocation(mTeleportLocation);
	player->GetMovementComponent().SetNewDestination(player, mTeleportLocation, mTeleportLocation, worldTime, 0.0f);

	Protocol::S2C_Teleport teleportPacket;
	teleportPacket.set_object_id(player->GetGameObjectID());
	teleportPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(mTeleportLocation));

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, teleportPacket);
	this->BrodcastPlayerViewers(sendBuffer);

	static bool spawn = false;
	if (false == spawn)
	{
		spawn = true;
		
		//std::shared_ptr<EnemyDarkKnight> rich = std::static_pointer_cast<EnemyDarkKnight>(world->SpawnActor<EnemyDarkKnight>(world, mTeleportLocation, Rotation(), Scale()));
	
		//std::shared_ptr<EnemyRichPhase1> rich = std::static_pointer_cast<EnemyRichPhase1>(world->SpawnActor<EnemyRichPhase1>(world, mTeleportLocation, Rotation(), Scale()));
		std::shared_ptr<EnemyRichPhase2> rich = std::static_pointer_cast<EnemyRichPhase2>(world->SpawnActor<EnemyRichPhase2>(world, mTeleportLocation, Rotation(), Scale()));
		//std::shared_ptr<EnemyRichPhase3> rich = std::static_pointer_cast<EnemyRichPhase3>(world->SpawnActor<EnemyRichPhase3>(world, mTeleportLocation, Rotation(), Scale()));

	}

	mUse = true;
}

void Portal::SetTeleportLocation(const FVector& inLocation)
{
	mTeleportLocation = inLocation;
}

BoxCollisionComponent* Portal::GetBoxCollisionComponent() const
{
	return static_cast<BoxCollisionComponent*>(this->mDefaultCollisionComponent);
}