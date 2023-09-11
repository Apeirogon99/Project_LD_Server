#include "pch.h"
#include "Wall.h"

Wall::Wall() : Actor(L"Wall")
{
	this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

Wall::~Wall()
{
}

void Wall::OnInitialization()
{
	this->SetActorType(static_cast<int8>(EActorType::Obstruction));
}

void Wall::OnDestroy()
{
}

void Wall::OnTick(const int64 inDeltaTime)
{
}

bool Wall::IsValid()
{
	return true;
}

void Wall::SetBoxCollisionExtent(FVector inExtent)
{
	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(inExtent);
}

BoxCollisionComponent* Wall::GetBoxCollisionComponent() const
{
	return static_cast<BoxCollisionComponent*>(this->mDefaultCollisionComponent);
}
