#include "pch.h"
#include "Wall.h"

Wall::Wall() : Obstruction()
{
}

Wall::~Wall()
{
}

void Wall::OnInitialization()
{
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