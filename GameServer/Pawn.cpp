#include "pch.h"
#include "Pawn.h"

Pawn::Pawn(const WCHAR* inName) : Actor(inName)
{
}

Pawn::~Pawn()
{
}

void Pawn::Initialization()
{
}

void Pawn::Destroy()
{
}

void Pawn::Tick()
{
}

bool Pawn::IsValid()
{
    return false;
}

void Pawn::SetMoveLocation(const Protocol::SVector& inMoveLocation)
{
    mMoveLocation.CopyFrom(inMoveLocation);
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
