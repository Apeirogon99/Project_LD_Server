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

void RemotePlayer::OnInitialization()
{
	SetTick(0, false);
	RemotePlayerRef remotePlayerRef = GetRemotePlayer();
	mInventory = std::make_shared<Inventory>(remotePlayerRef, 12, 7);
	mCharacter = std::make_shared<Character>(remotePlayerRef);
}

void RemotePlayer::OnDestroy()
{
	
}

void RemotePlayer::OnTick(const int64 inDeltaTime)
{

}

bool RemotePlayer::IsValid()
{
	return true;
}

bool RemotePlayer::LoadRemotePlayer(const Token& inToken, WorldRef inWorld)
{

	mWorld = inWorld;
	mToken = inToken;

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

	if (false == Handle_LoadCharacter_Request(packetSession, mToken.GetCharacterID()))
	{
		return false;
	}

	if (false == Handle_LoadInventory_Requset(packetSession, mToken.GetCharacterID()))
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

	WorldPtr world = this->GetWorldRef().lock();
	if (nullptr == world)
	{
		return;
	}

	CharacterPtr character = GetCharacter();
	if (nullptr == character)
	{
		return;
	}
	//Load
	character->UpdateStats();

	Protocol::S2C_EnterGameServer enterPacket;
	enterPacket.set_remote_id(this->GetGameObjectID());
	enterPacket.mutable_character_data()->CopyFrom(character->GetCharacterData());
	GetInventory()->LoadItem(enterPacket.mutable_item());
	GetInventory()->LoadEqipment(enterPacket.mutable_eqipment());
	enterPacket.mutable_transform()->CopyFrom(character->GetTransform());
	enterPacket.set_error(false);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, enterPacket);
	packetSession->Send(sendBuffer);

	world->VisibleAreaInit(playerState);
}

void RemotePlayer::InitTask(GameTaskPtr& inGameTask)
{
	if (nullptr == inGameTask)
	{
		return;
	}

	GameObjectPtr remoteGameObject = this->GetGameObjectPtr();
	inGameTask->PushTask(remoteGameObject);

	GameObjectPtr characterGameObject = this->GetCharacter()->GetGameObjectPtr();
	inGameTask->PushTask(characterGameObject);

	GameObjectPtr inventoryGameObject = this->GetInventory()->GetGameObjectPtr();
	inGameTask->PushTask(inventoryGameObject);
}

void RemotePlayer::DestroyTask(GameTaskPtr& inGameTask)
{
	if (nullptr == inGameTask)
	{
		return;
	}

	GameObjectPtr remoteGameObject = this->GetGameObjectPtr();
	inGameTask->ReleaseTask(remoteGameObject);

	GameObjectPtr characterGameObject = this->GetCharacter()->GetGameObjectPtr();
	inGameTask->ReleaseTask(characterGameObject);

	GameObjectPtr inventoryGameObject = this->GetInventory()->GetGameObjectPtr();
	inGameTask->ReleaseTask(inventoryGameObject);
}

void RemotePlayer::BrodcastViewers(SendBufferPtr inSendBuffer)
{
	Viewers& viewers = GetViewers();
	for (auto viewer : viewers)
	{
		viewer->Send(inSendBuffer);
	}
}