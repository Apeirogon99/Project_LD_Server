#include "pch.h"
#include "RemotePlayer.h"

//RemotePlayer::RemotePlayer(const PlayerStateRef& inPlayerState, const int64 inRemoteID) : GameObject(L"RemotePlayer"), mPlayerState(inPlayerState), mRemoteID(inRemoteID)
//{
//
//}

RemotePlayer::RemotePlayer(const PlayerStateRef& inPlayerState) : GameObject(L"RemotePlayer"), mPlayerState(inPlayerState)
{
}

RemotePlayer::~RemotePlayer()
{

}

void RemotePlayer::Initialization()
{
	RemotePlayerRef remotePlayerRef = GetRemotePlayer();
	mInventory = std::make_shared<Inventory>(remotePlayerRef, 12, 7);
	mCharacter = std::make_shared<Character>(remotePlayerRef);
}

void RemotePlayer::Destroy()
{
	
}

void RemotePlayer::Tick()
{

}

bool RemotePlayer::IsValid()
{
	return true;
}

bool RemotePlayer::LoadRemotePlayer(const int32 inServerID, const int32 inCharacterID)
{
	PlayerStatePtr playerState = mPlayerState.lock();
	if (nullptr == playerState)
	{
		return false;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	if (nullptr == packetSession)
	{
		return false;
	}

	if (false == Handle_LoadCharacter_Request(packetSession, inCharacterID))
	{
		return false;
	}

	if (false == Handle_LoadInventory_Requset(packetSession, inCharacterID))
	{
		return false;
	}

	return true;
}

void RemotePlayer::LoadComplete()
{
	PlayerStatePtr playerState = mPlayerState.lock();
	if (nullptr == playerState)
	{
		return;
	}

	if (false == GetCharacter()->IsLoad())
	{
		return;
	}

	if (false == GetInventory()->IsLoad())
	{
		return;
	}


	Protocol::S2C_EnterGameServer enterPacket;
	enterPacket.set_remote_id(this->GetGameObjectID());
	enterPacket.mutable_character_data()->CopyFrom(GetCharacter()->GetCharacterData());
	GetInventory()->LoadItem(enterPacket.mutable_item());
	GetInventory()->LoadEqipment(enterPacket.mutable_eqipment());
	enterPacket.mutable_transform()->CopyFrom(GetCharacter()->GetTransform());
	enterPacket.set_error(false);


	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, enterPacket);
	packetSession->Send(sendBuffer);
}

void RemotePlayer::InitTask(GameTaskPtr& inGameTask)
{
	if (nullptr == inGameTask)
	{
		return;
	}

	inGameTask->PushTask(this->GetGameObjectPtr());
	inGameTask->PushTask(this->GetCharacter());
	inGameTask->PushTask(this->GetInventory());
}

void RemotePlayer::DestroyTask(GameTaskPtr& inGameTask)
{
	if (nullptr == inGameTask)
	{
		return;
	}

	inGameTask->ReleaseTask(this->GetInventory());
	inGameTask->ReleaseTask(this->GetCharacter());
	inGameTask->ReleaseTask(this->GetGameObjectPtr());
}
