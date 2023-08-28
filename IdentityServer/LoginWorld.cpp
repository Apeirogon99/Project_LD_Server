#include "pch.h"
#include "LoginWorld.h"

LoginWorld::LoginWorld() : World(L"LoginWorld")
{
}

LoginWorld::~LoginWorld()
{

}

void LoginWorld::OnInitialization()
{
	IdentityTaskPtr taskManager = std::static_pointer_cast<IdentityTask>(this->GetTaskManagerRef().lock());
	if (nullptr == taskManager)
	{
		return;
	}
	GameObjectRef owner = this->GetGameObjectRef();

	mLoginRoom = std::make_shared<LoginRoom>();
	mLoginRoom->SetOwner(owner);
	taskManager->PushTask(this->GetLoginRoom()->GetGameObjectPtr());

	mSelectRoom = std::make_shared<SelectRoom>();
	mSelectRoom->SetOwner(owner);
	taskManager->PushTask(this->GetSelectRoom()->GetGameObjectPtr());

	mCustomRoom = std::make_shared<CustomRoom>();
	mCustomRoom->SetOwner(owner);
	taskManager->PushTask(this->GetCustomRoom()->GetGameObjectPtr());

}

void LoginWorld::OnDestroy()
{
	IdentityTaskPtr taskManager = std::static_pointer_cast<IdentityTask>(this->GetTaskManagerRef().lock());
	if (nullptr == taskManager)
	{
		return;
	}

	taskManager->ReleaseTask(this->GetLoginRoom()->GetGameObjectPtr());
	mLoginRoom.reset();

	taskManager->ReleaseTask(this->GetSelectRoom()->GetGameObjectPtr());
	mSelectRoom.reset();

	taskManager->ReleaseTask(this->GetCustomRoom()->GetGameObjectPtr());
	mCustomRoom.reset();

}

void LoginWorld::OnTick(const int64 inDeltaTime)
{

}

bool LoginWorld::IsValid()
{
	return true;
}

void LoginWorld::EnterWorld(PlayerStatePtr inPlayerState)
{
	IdentityTaskPtr task = std::static_pointer_cast<IdentityTask>(this->GetTaskManagerRef().lock());
	if (nullptr == task)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::make_shared<LoginRemotePlayer>();
	inPlayerState->SetRemotePlayer(remotePlayer);
	task->CreateGameObject(remotePlayer->GetGameObjectPtr());

	const int64 gameObjectID = remotePlayer->GetGameObjectID();
	std::pair<int64, LoginRemotePlayerPtr> newRemotePlayer = std::make_pair(gameObjectID, remotePlayer);
	auto insertResult = mRemotePlayers.insert(newRemotePlayer);

	Protocol::S2C_EnterIdentityServer enterPacket;
	enterPacket.set_error(insertResult.second);

	PacketSessionPtr session = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, enterPacket);
	session->Send(sendBuffer);
}

void LoginWorld::LeaveWorld(PlayerStatePtr inPlayerState)
{
	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
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

	Protocol::S2C_LeaveIdentityServer leavePacket;
	leavePacket.set_error(eraseResult == 0 ? false : true);

	PacketSessionPtr session = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, leavePacket);
	session->Send(sendBuffer);
}

void LoginWorld::SetServerInfo(const std::vector<Protocol::SServerInfo>& inServerInfo)
{
	mServerInfo = inServerInfo;
}

WorldRef LoginWorld::GetWorldRef()
{
	return std::static_pointer_cast<World>(shared_from_this());
}

LoginRoomPtr LoginWorld::GetLoginRoom()
{
	return mLoginRoom;
}

SelectRoomPtr LoginWorld::GetSelectRoom()
{
	return mSelectRoom;
}

CustomRoomPtr LoginWorld::GetCustomRoom()
{
	return mCustomRoom;
}

const std::vector<Protocol::SServerInfo>& LoginWorld::GetServerInfo()
{
	return mServerInfo;
}

bool LoginWorld::IsValidPlayer(RemotePlayerPtr inRemotePlayer)
{
	const int64 gameObjectID = inRemotePlayer->GetGameObjectID();
	auto findResult = mRemotePlayers.find(gameObjectID);
	if (findResult == mRemotePlayers.end())
	{
		return false;
	}

	return true;
}
