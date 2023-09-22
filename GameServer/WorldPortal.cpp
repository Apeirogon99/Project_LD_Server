#include "pch.h"
#include "WorldPortal.h"

WorldPortal::WorldPortal():SphereTrigger(), mTeleportLevel(), mMaxTeleportTime(10000), mCurTeleportTime(0), mIsTeleport(false)
{
}

WorldPortal::~WorldPortal()
{
}

void WorldPortal::OnInitialization()
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

void WorldPortal::OnAppearActor(ActorPtr inAppearActor)
{
}

void WorldPortal::OnDisAppearActor(ActorPtr inDisappearActor)
{
}

void WorldPortal::OnBeginOverlap(ActorPtr inBeginOverlapActor)
{
	if (mMaxOverlap == mOverlapActor.size() && false == mIsTeleport)
	{
		EnterWorldTeleport();
	}
}

void WorldPortal::OnEndOverlap(ActorPtr inEndOverlapActor)
{
	if (true == mIsTeleport)
	{
		EndWorldTeleport();
	}
}

void WorldPortal::OnOverlapTick(const int64 inDeltaTime)
{
	if (true == mIsTeleport)
	{
		mCurTeleportTime += inDeltaTime;
		if (mCurTeleportTime >= mMaxTeleportTime)
		{
			this->WorldTeleport();
		}
		else
		{
			this->EnterWorldTeleport();
		}
	}
}

void WorldPortal::EnterWorldTeleport()
{
	mIsTeleport = true;

	float time = (mMaxTeleportTime - mCurTeleportTime) / 1000.0f;
	std::string title = "잠시후 다음 월드로 이동됩니다.";

	Protocol::S2C_EnterPortal enterPortalPacket;
	enterPortalPacket.set_title(title);
	enterPortalPacket.set_time(static_cast<int64>(time + 1));

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, enterPortalPacket);
	this->BroadCastOverlap(sendBuffer);
}

void WorldPortal::EndWorldTeleport()
{
	mIsTeleport = false;
	mCurTeleportTime = 0;

	Protocol::S2C_LeavePortal leavePortalPacket;

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePortalPacket);
	this->BroadCastOverlap(sendBuffer);
}

void WorldPortal::WorldTeleport()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	GameWorldPtr teleportWorld = mTeleportWorld.lock();
	if (nullptr == teleportWorld)
	{
		return;
	}

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
		const int64& remoteID = remotePlayer->GetGameObjectID();

		world->LevelTravel(teleportWorld, remoteID);

		Location randomLocation = Random::GetRandomVectorInRange2D(mTeleportLocation, 300.0f);
		character->GetMovementComponent().SetNewDestination(character->GetActorPtr(), randomLocation, randomLocation, teleportWorld->GetWorldTime(), 0.0f);

	}

	Protocol::S2C_TravelLevel travelPacket;
	travelPacket.set_level(this->mTeleportLevel);
	travelPacket.set_time_stamp(worldTime);
	travelPacket.set_error(1);

	SendBufferPtr sendBuffer = CommonServerPacketHandler::MakeSendBuffer(nullptr, travelPacket);
	this->BroadCastOverlap(sendBuffer);

	mOverlapActor.clear();

	world->PushTask(world->GetNextWorldTime(), &World::DestroyActor, this->GetGameObjectID());
}

void WorldPortal::SetWaitWorldTeleprotTime(int64 inMaxTeleportTime)
{
	mMaxTeleportTime = inMaxTeleportTime;
}

void WorldPortal::SetTeleportWorld(GameWorldRef inGameWorldRef)
{
	mTeleportWorld = inGameWorldRef;
}

void WorldPortal::SetTeleportLevel(const std::string& inLevel)
{
	mTeleportLevel = inLevel;
}

void WorldPortal::SetTeleportLocation(const FVector& inLocation)
{
	mTeleportLocation = inLocation;
}

void WorldPortal::BroadCastOverlap(SendBufferPtr inSendBuffer)
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
