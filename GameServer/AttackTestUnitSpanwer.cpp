#include "pch.h"
#include "AttackTestUnitSpanwer.h"
#include "AttackTestUnit.h"

AttackTestUnitSpanwer::AttackTestUnitSpanwer() : Actor(L"AttackTestUnitSpanwer"), mMaxSpawnRange(0.0f), mMaxGroundRange(0.0f), mMaxUnit(0), mAddUint(0), mCountUnit(0), mCreateSpawnCurrentTime(0), mCreateSpawnCoolTime(0)
{
	this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

AttackTestUnitSpanwer::~AttackTestUnitSpanwer()
{
}

void AttackTestUnitSpanwer::OnInitialization()
{
	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}

	SetTick(true, SYSTEM_TICK);
	this->SetActorType(static_cast<uint8>(EActorType::Visible));

	mMaxSpawnRange	= 800.0f;
	mMaxGroundRange = 1000.0f;

	mMaxUnit	= 1;
	mAddUint	= 1;
	mCountUnit	= 0;

	mCreateSpawnCurrentTime = 0;
	mCreateSpawnCoolTime	= 5000;
}

void AttackTestUnitSpanwer::OnDestroy()
{
}

void AttackTestUnitSpanwer::OnTick(const int64 inDeltaTime)
{

	this->SendQueue();

	mCreateSpawnCurrentTime += inDeltaTime;
	if (mCreateSpawnCurrentTime <= mCreateSpawnCoolTime)
	{
		return;
	}
	mCreateSpawnCurrentTime = 0;

	int32 createUnitCount = 0;
	while (createUnitCount != mAddUint)
	{
		if (mMaxUnit == mCountUnit)
		{
			return;
		}

		this->CreateUnit();

		createUnitCount++;
		mCountUnit++;
	}

}

bool AttackTestUnitSpanwer::IsValid()
{
	return false;
}

void AttackTestUnitSpanwer::OnAppearActor(ActorPtr inAppearActor)
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
}

void AttackTestUnitSpanwer::OnDisAppearActor(ActorPtr inDisappearActor)
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
}

void AttackTestUnitSpanwer::SendQueue()
{
	if (mMovementPacket.object_id_size() != 0)
	{
		this->BrodcastPlayerViewers(GameServerPacketHandler::MakeSendBuffer(nullptr, mMovementPacket));
		mMovementPacket.Clear();
	}

	if (mBeginOverlapPacket.object_id_size() != 0)
	{
		this->BrodcastPlayerViewers(GameServerPacketHandler::MakeSendBuffer(nullptr, mBeginOverlapPacket));
		mBeginOverlapPacket.Clear();
	}

	if (mEndOverlapPacket.object_id_size() != 0)
	{
		this->BrodcastPlayerViewers(GameServerPacketHandler::MakeSendBuffer(nullptr, mEndOverlapPacket));
		mEndOverlapPacket.Clear();
	}
}

void AttackTestUnitSpanwer::CreateUnit()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	Location	spawnLocation = Random::GetRandomVectorInRange2D(this->GetLocation(), mMaxSpawnRange);
	FRotator	spawnRotation(0.0f, static_cast<float>(Random::GetRealUniformDistribution(-180, 180)), 0.0f);

	ActorPtr actor = world->SpawnActor<AttackTestUnit>(this->GetGameObjectRef(), spawnLocation, spawnRotation, Scale(1.0f, 1.0f, 1.0f));
	std::shared_ptr<AttackTestUnit> newUnit = std::static_pointer_cast<AttackTestUnit>(actor);
	if (nullptr == newUnit)
	{
		return;
	}
}

void AttackTestUnitSpanwer::MakeMovementPacket(const int64& inObjectID, const FVector& inLocation, const FRotator& inRotation, const int64& inTimeStamp)
{
	mMovementPacket.add_object_id(inObjectID);
	mMovementPacket.add_location()->CopyFrom(PacketUtils::ToSVector(inLocation));
	mMovementPacket.add_rotation()->CopyFrom(PacketUtils::ToSRotator(inRotation));
	mMovementPacket.add_timestamp(inTimeStamp);
}

void AttackTestUnitSpanwer::MakeBeginOverlapPacket(const int64& inObjectID)
{
	mBeginOverlapPacket.add_object_id(inObjectID);
}

void AttackTestUnitSpanwer::MakeEndOverlapPacket(const int64& inObjectID)
{
	mEndOverlapPacket.add_object_id(inObjectID);
}
