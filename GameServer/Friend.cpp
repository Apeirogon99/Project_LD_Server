#include "pch.h"
#include "Friend.h"

Friend::Friend() : GameObject(L"Friend"), mIsLoad(false)
{
}

Friend::~Friend()
{
}

void Friend::OnInitialization()
{
	this->SetTick(false);
}

void Friend::OnDestroy()
{
	mFriends.clear();
}

void Friend::OnTick(const int64 inDeltaTime)
{
	//NONE
}

bool Friend::IsValid()
{
	return mIsLoad;
}

void Friend::SetLoadFriend(bool inIsLoad)
{
	mIsLoad = inIsLoad;
}

void Friend::LoadFriendList(const int32 inListType)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr remoteClient = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	const int64 characterID = remotePlayer->GetToken().GetCharacterID();
	Handle_LoadFriendList_Request(remoteClient, characterID, inListType);
}

void Friend::RequestFriend(const std::string inFriendName, const int32 inAction)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr remoteClient = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	const int64 characterID = remotePlayer->GetToken().GetCharacterID();
	Handle_RequestFriend_Request(remoteClient, characterID, inFriendName, inAction);
}

void Friend::BlockFriend(const std::string inFriendName, const int32 inAction)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr remoteClient = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	const int64 characterID = remotePlayer->GetToken().GetCharacterID();
	Handle_BlockFriend_Request(remoteClient, characterID, inFriendName, inAction);
}

void Friend::FindFriend(const std::string inFriendName)
{
}

bool Friend::IsFriend(const int64& inFriendCharacterID)
{
	return false;
}

void Friend::NotifyConnectToFriend()
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr remoteClient = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	const int64 characterID = remotePlayer->GetToken().GetCharacterID();
	Handle_ConnectLoadFriendList_Request(remoteClient, characterID);
}

void Friend::DelegateOnlineFriend(const int64& inFriendCharacterID, const std::string& inFriendName)
{
	auto findPos = mFriends.find(inFriendCharacterID);
	if (findPos == mFriends.end())
	{
		return;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr remoteClient = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	Protocol::S2C_ConnectFriend connectFriendPacket;
	connectFriendPacket.set_nick_name(inFriendName);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(remoteClient, connectFriendPacket);
	remoteClient->Send(sendBuffer);
}

void Friend::NotifyDisConnectToFriend()
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr remoteClient = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	const int64 characterID = remotePlayer->GetToken().GetCharacterID();
	Handle_DisConnectLoadFriendList_Request(remoteClient, characterID);
}

void Friend::DelegateOfflineFriend(const int64& inFriendCharacterID, const std::string& inFriendName)
{
	auto findPos = mFriends.find(inFriendCharacterID);
	if (findPos == mFriends.end())
	{
		return;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr remoteClient = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	Protocol::S2C_DisConnectFriend disconnectFriendPacket;
	disconnectFriendPacket.set_nick_name(inFriendName);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(remoteClient, disconnectFriendPacket);
	remoteClient->Send(sendBuffer);
}

void Friend::PushFriend(const std::set<int64>& inFriends)
{
	mFriends.clear();
	mFriends = inFriends;
}
