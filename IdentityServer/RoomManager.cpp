#include "pch.h"
#include "RoomManager.h"

RoomManager::RoomManager() : GameObject(L"RoomManager"), mRoomType(ERoomType::None)
{
}

RoomManager::~RoomManager()
{
}

void RoomManager::OnInitialization()
{
}

void RoomManager::OnDestroy()
{
}

void RoomManager::OnTick(const int64 inDeltaTime)
{
}

bool RoomManager::IsValid()
{
	return (this->GetOwner().lock() != nullptr);
}

bool RoomManager::RoomCmp(const ERoomType& inRoomType)
{
	return mRoomType == inRoomType;
}

bool RoomManager::CanChanageRoom(const ERoomType& inRoomType)
{
	return true;
}

void RoomManager::SetRoomType(const ERoomType& inRoomType)
{
	mRoomType = inRoomType;
}

const ERoomType& RoomManager::GetRoomType() const
{
	return mRoomType;
}
