#include "pch.h"
#include "World.h"

World::World(IdentityTaskPtr inIdentityTask) : GameObject(L"World"), mIdentityTask(inIdentityTask)
{

}

World::~World()
{

}

void World::Initialization() 
{
	if (nullptr == mIdentityTask)
	{
		return;
	}

	mIdentityTask->PushTask(std::make_shared<LoginRoom>(GetWorldRef())->GetGameObjectPtr());
	mIdentityTask->PushTask(std::make_shared<SelectRoom>(GetWorldRef())->GetGameObjectPtr());
	mIdentityTask->PushTask(std::make_shared<CustomRoom>(GetWorldRef())->GetGameObjectPtr());

}

void World::Destroy()
{
	if (nullptr == mIdentityTask)
	{
		return;
	}

	GameObjectPtr loginRoom;
	GameObjectPtr selectRoom;
	GameObjectPtr customRoom;

	mIdentityTask->FindTask(L"LoginRoom", loginRoom);
	mIdentityTask->FindTask(L"SelectRoom", selectRoom);
	mIdentityTask->FindTask(L"CustomRoom", customRoom);

	mIdentityTask->ReleaseTask(loginRoom);
	mIdentityTask->ReleaseTask(selectRoom);
	mIdentityTask->ReleaseTask(customRoom);

}

void World::Tick()
{

}

bool World::IsValid()
{
	return true;
}

void World::Enter(PlayerStatePtr inPlayerState)
{
	if (nullptr == mIdentityTask)
	{
		return;
	}

	RemotePlayerPtr remotePlayer = std::make_shared<RemotePlayer>();
	inPlayerState->SetRemotePlayer(remotePlayer);
	mIdentityTask->CreateGameObject(remotePlayer->GetGameObjectPtr());

	const int64 gameObjectID = remotePlayer->GetGameObjectID();
	std::pair<int64, RemotePlayerPtr> newRemotePlayer = std::make_pair(gameObjectID, remotePlayer);
	auto insertResult = mRemotePlayers.insert(newRemotePlayer);

	if (false == insertResult.second)
	{
		//Error
	}

	remotePlayer->SetRoomType(ERoomType::LoginRoom);

	Protocol::S2C_EnterIdentityServer enterPacket;
	enterPacket.set_remote_id(gameObjectID);
	enterPacket.set_error(insertResult.second);

	PacketSessionPtr session = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, enterPacket);
	session->Send(sendBuffer);
}

void World::Leave(PlayerStatePtr inPlayerState)
{
	RemotePlayerPtr remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	if (false == IsValidPlayer(remotePlayer))
	{
		return;
	}

	const int64 gameObjectID = remotePlayer->GetGameObjectID();
	size_t eraseResult = mRemotePlayers.erase(gameObjectID);
	if (eraseResult == 0)
	{
		//Error
	}

	Protocol::S2C_LeaveIdentityServer leavePacket;
	leavePacket.set_remote_id(gameObjectID);
	leavePacket.set_error(eraseResult == 0 ? false : true);

	PacketSessionPtr session = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, leavePacket);
	session->Send(sendBuffer);
}

WorldRef World::GetWorldRef()
{
	return std::static_pointer_cast<World>(shared_from_this());
}

LoginRoomPtr World::GetLoginRoom()
{
	if (nullptr == mIdentityTask)
	{
		return nullptr;
	}

	GameObjectPtr object;
	mIdentityTask->FindTask(L"LoginRoom", object);
	if (nullptr == object)
	{
		return nullptr;
	}

	LoginRoomPtr loginRoom = std::static_pointer_cast<LoginRoom>(object);
	if (nullptr == loginRoom)
	{
		return nullptr;
	}

	return loginRoom;
}

SelectRoomPtr World::GetSelectRoom()
{
	if (nullptr == mIdentityTask)
	{
		return nullptr;
	}

	GameObjectPtr object;
	mIdentityTask->FindTask(L"SelectRoom", object);
	if (nullptr == object)
	{
		return nullptr;
	}

	SelectRoomPtr selectRoom = std::static_pointer_cast<SelectRoom>(object);
	if (nullptr == selectRoom)
	{
		return nullptr;
	}

	return selectRoom;
}

CustomRoomPtr World::GetCustomRoom()
{
	if (nullptr == mIdentityTask)
	{
		return nullptr;
	}

	GameObjectPtr object;
	mIdentityTask->FindTask(L"CustomRoom", object);
	if (nullptr == object)
	{
		return nullptr;
	}

	CustomRoomPtr customRoom = std::static_pointer_cast<CustomRoom>(object);
	if (nullptr == customRoom)
	{
		return nullptr;
	}

	return customRoom;
}

bool World::IsValidPlayer(RemotePlayerPtr inRemotePlayer)
{
	const int64 gameObjectID = inRemotePlayer->GetGameObjectID();
	auto findResult = mRemotePlayers.find(gameObjectID);
	if (findResult == mRemotePlayers.end())
	{
		return false;
	}

	return true;
}
