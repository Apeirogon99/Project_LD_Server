#include "pch.h"
#include "GameRemotePlayer.h"

GameRemotePlayer::GameRemotePlayer() : RemotePlayer()
{
}

GameRemotePlayer::~GameRemotePlayer()
{

}

void GameRemotePlayer::OnInitialization()
{
	GameObjectRef owner = this->GetGameObjectRef();
	mInventory = std::make_shared<Inventory>(12, 7);
	mInventory->SetOwner(owner);

	mPlayerCharacter = std::make_shared<PlayerCharacter>();
	mPlayerCharacter->SetOwner(owner);

	GameTaskPtr taskManager = std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());
	if (nullptr == taskManager)
	{
		return;
	}

	GameObjectPtr remoteGameObject = this->GetGameObjectPtr();
	taskManager->CreateGameObject(remoteGameObject);

	GameObjectPtr characterGameObject = this->GetCharacter()->GetGameObjectPtr();
	taskManager->PushTask(characterGameObject);

	GameObjectPtr inventoryGameObject = this->GetInventory()->GetGameObjectPtr();
	taskManager->CreateGameObject(inventoryGameObject);
}

void GameRemotePlayer::OnDestroy()
{
	GameTaskPtr taskManager = std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());
	if (nullptr == taskManager)
	{
		return;
	}

	GameObjectPtr remoteGameObject = this->GetGameObjectPtr();
	taskManager->DestroyGameObject(remoteGameObject);

	GameObjectPtr characterGameObject = this->GetCharacter()->GetGameObjectPtr();
	taskManager->ReleaseTask(characterGameObject);

	GameObjectPtr inventoryGameObject = this->GetInventory()->GetGameObjectPtr();
	taskManager->DestroyGameObject(inventoryGameObject);
}

void GameRemotePlayer::OnTick(const int64 inDeltaTime)
{

}

bool GameRemotePlayer::IsValid()
{
	return true;
}

bool GameRemotePlayer::LoadRemotePlayer(const Token& inToken, GameWorldRef inWorld)
{

	mWorld = inWorld;
	mToken = inToken;

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(this->GetRemoteClient().lock());
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

void GameRemotePlayer::OnLoadComplete()
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(this->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return;
	}

	if (false == GetCharacter()->IsValid())
	{
		return;
	}

	if (false == GetInventory()->IsValid())
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(this->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	PlayerCharacterPtr character = GetCharacter();
	if (nullptr == character)
	{
		return;
	}

	Protocol::S2C_EnterGameServer enterPacket;
	enterPacket.set_remote_id(this->GetGameObjectID());
	enterPacket.mutable_character_data()->CopyFrom(character->GetCharacterData());
	GetInventory()->LoadItem(enterPacket.mutable_item());
	GetInventory()->LoadEqipment(enterPacket.mutable_eqipment());
	enterPacket.mutable_transform()->CopyFrom(PacketUtils::ToSTransform(character->GetTransform()));
	enterPacket.set_error(false);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, enterPacket);
	packetSession->Send(sendBuffer);

	world->VisibleAreaInit(playerState);
}