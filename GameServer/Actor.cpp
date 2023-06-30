#include "pch.h"
#include "Actor.h"

Actor::Actor(const WCHAR* inActorName) : GameObject(inActorName)
{
	
}

Actor::~Actor()
{
	
}

void Actor::OnInitialization()
{
	SetTick(0, false);
}

void Actor::OnDestroy()
{

}

void Actor::OnTick(const int64 inDeltaTime)
{
}

bool Actor::IsValid()
{
	return true;
}

void Actor::CloseToPlayer(PlayerStatePtr inClosePlayerState)
{
	CharacterPtr closeCharacter = inClosePlayerState->GetRemotePlayer()->GetCharacter();
	if (nullptr == this || nullptr == closeCharacter)
	{
		return;
	}

	const float possibleVisbleLength = 2000.0f;
	const float playerDistance = FVector::Distance(PacketUtils::ToFVector(this->GetLocation()), PacketUtils::ToFVector(closeCharacter->GetLocation()));
	if (playerDistance < possibleVisbleLength)
	{
		AppearActor(inClosePlayerState);
	}
	else
	{
		DisAppearActor(inClosePlayerState);
	}
}

void Actor::SetOwner(GameObjectRef inOwner)
{
	mOwner = inOwner;
}

void Actor::SetWorld(WorldRef inWorld)
{
	mWorld = inWorld;
}

void Actor::SetTransform(const Protocol::SVector& inLocation, const Protocol::SRotator& inRotation, const Protocol::SVector& inScale)
{
	SetLocation(inLocation);
	SetRotation(inRotation);
	SetScale(inScale);
}

void Actor::SetTransform(const Protocol::STransform& inTransform)
{
	mTransfrom = inTransform;
}

void Actor::SetTransform(const FVector& inLocation, const FRotator& inRotation, const FVector& inScale)
{
	this->mTransfrom.mutable_location()->CopyFrom(PacketUtils::ToSVector(inLocation));
	this->mTransfrom.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(inRotation));
	this->mTransfrom.mutable_scale()->CopyFrom(PacketUtils::ToSVector(inScale));
}

void Actor::SetLocation(const float inX, const float inY, const float inZ)
{
	Protocol::SVector* location = mTransfrom.mutable_location();
	location->set_x(inX);
	location->set_y(inY);
	location->set_z(inZ);
}

void Actor::SetLocation(const Protocol::SVector& inLocation)
{
	Protocol::SVector* location = mTransfrom.mutable_location();
	location->CopyFrom(inLocation);
}

void Actor::SetLocation(const FVector& inLocation)
{
	this->mTransfrom.mutable_location()->CopyFrom(PacketUtils::ToSVector(inLocation));
}

void Actor::SetRotation(const float inRoll, const float inPitch, const float inYaw)
{
	Protocol::SRotator* rotation = mTransfrom.mutable_rotation();
	rotation->set_roll(inRoll);
	rotation->set_pitch(inPitch);
	rotation->set_yaw(inYaw);
}

void Actor::SetRotation(const Protocol::SRotator& inRotator)
{
	Protocol::SRotator* rotation = mTransfrom.mutable_rotation();
	rotation->CopyFrom(inRotator);
}

void Actor::SetRotation(const FRotator& inRotator)
{
	this->mTransfrom.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(inRotator));
}

void Actor::SetScale(const float inX, const float inY, const float inZ)
{
	Protocol::SVector* scale = mTransfrom.mutable_scale();
	scale->set_x(inX);
	scale->set_y(inY);
	scale->set_z(inZ);
}

void Actor::SetScale(const Protocol::SVector& inScale)
{
	Protocol::SVector* scale = mTransfrom.mutable_scale();
	scale->CopyFrom(inScale);
}

void Actor::SetScale(const FVector& inScale)
{
	this->mTransfrom.mutable_scale()->CopyFrom(PacketUtils::ToSVector(inScale));
}

void Actor::SetVelocity(const float inX, const float inY, const float inZ)
{
	mVelocity.set_x(inX);
	mVelocity.set_y(inY);
	mVelocity.set_z(inZ);
}

void Actor::SetVelocity(const Protocol::SVector& inVelocity)
{
	mVelocity = inVelocity;
}

void Actor::SetVelocity(const FVector& inVelocity)
{
	this->mVelocity = PacketUtils::ToSVector(inVelocity);
}

void Actor::BrodcastViewers(SendBufferPtr inSendBuffer)
{
	Viewers& viewers = GetViewers();
	for (auto viewer : viewers)
	{
		viewer->Send(inSendBuffer);
	}
}
