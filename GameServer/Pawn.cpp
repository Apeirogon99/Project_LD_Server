#include "pch.h"
#include "Pawn.h"

Pawn::Pawn(const WCHAR* inName) : Actor(inName), mMoveSyncTick(0)
{
}

Pawn::~Pawn()
{
}

void Pawn::OnInitialization()
{
	SetTick(0, false);
}

void Pawn::OnDestroy()
{
}

void Pawn::OnTick(const int64 inDeltaTime)
{
}

bool Pawn::IsValid()
{
    return false;
}

void Pawn::MoveDestination(Protocol::C2S_MovementCharacter inPacket)
{
	Protocol::SVector	serverLocation	= this->GetLocation();
	Protocol::SVector	clientLocation	= inPacket.cur_location();
	Protocol::SVector	moveLocation	= inPacket.move_location();
	const int64			timestamp		= inPacket.timestamp();

	//const float closeToDestination = MAX_LOCATION_DISTANCE;
	//const float locationDistance = FVector::Distance2D(serverLocation, clientLocation);
	//if (locationDistance < closeToDestination)
	//{
	//}

	//this->SetLocation(clientLocation);
	this->SetMoveLocation(moveLocation);
	this->SetMoveLastTick(timestamp);

	OnMovement(timestamp);

}

void Pawn::MovingDestination(const int64 inTime)
{
	FVector curLocation		= PacketUtils::ToFVector(this->GetLocation());
	FVector moveLocation	= PacketUtils::ToFVector(this->GetMoveLocation());

	float	maxVelocity = this->GetStats().GetMovementSpeed();
	float	duration	= static_cast<float>((inTime - this->GetMoveLastTick())) / 1000.0f;

	FVector	direction	= moveLocation - curLocation;
	direction.Normalize();

	FRotator rotation = direction.Rotator();
	FVector velocity = direction * maxVelocity;

	FVector	deadReckoningLocation = curLocation + (velocity * duration);

	//GameObjectLog(L"[Move] (%5.6f:%5.6f:%5.6f)\n", deadReckoningLocation.GetX(), deadReckoningLocation.GetY(), deadReckoningLocation.GetZ());

	SetLocation(deadReckoningLocation);
	SetRotation(rotation);
	SetMoveLastTick(inTime);
}

void Pawn::SetMoveLocation(const Protocol::SVector& inMoveLocation)
{
    mMoveLocation.CopyFrom(inMoveLocation);
}

void Pawn::SetMoveLocation(const FVector& inMoveLocation)
{
	mMoveLocation.CopyFrom(PacketUtils::ToSVector(inMoveLocation));
}

void Pawn::SetMoveLastTick(const int64 inMoveLastTick)
{
    mMoveLastTick = inMoveLastTick;
}

void Pawn::SetAnimationType(const int32 inAnimationType)
{
    mAnimationType = inAnimationType;
}

void Pawn::SetAnimationLastTick(const int64 inAnimationLastTick)
{
    mAnimationLastTick = inAnimationLastTick;
}

void Pawn::SetStats(const Stats& inStats)
{
	mStats = inStats;
}
