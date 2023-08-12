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
	GameTaskPtr taskManager = std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());
	if (nullptr == taskManager)
	{
		return;
	}

	GameWorldPtr world = taskManager->GetWorld();
	if (nullptr == world)
	{
		return;
	}

	GameObjectRef owner = this->GetGameObjectRef();

	mInventory = std::make_shared<Inventory>(12, 7);
	mInventory->SetOwner(owner);
	taskManager->PushTask(this->GetInventory()->GetGameObjectPtr());

	mFriend = std::make_shared<Friend>();
	mFriend->SetOwner(owner);
	taskManager->PushTask(this->GetFriend()->GetGameObjectPtr());

	mParty = std::make_shared<Party>();
	mParty->SetOwner(owner);
	taskManager->PushTask(this->GetParty()->GetGameObjectPtr());

	mSkill = std::make_shared<Skill>();
	mSkill->SetOwner(owner);
	taskManager->PushTask(this->GetSkill()->GetGameObjectPtr());

	this->mPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(world->SpawnActor<PlayerCharacter>(owner, Location(), Rotation(), Scale()));
}

void GameRemotePlayer::OnDestroy()
{
	GameTaskPtr taskManager = std::static_pointer_cast<GameTask>(this->GetTaskManagerRef().lock());
	if (nullptr == taskManager)
	{
		return;
	}

	GameWorldPtr world = taskManager->GetWorld();
	if (nullptr == world)
	{
		return;
	}

	world->DestroyActor(this->GetCharacter()->GetGameObjectID());
	this->mPlayerCharacter.reset();

	taskManager->ReleaseTask(this->GetInventory()->GetGameObjectPtr());
	this->mInventory.reset();

	taskManager->ReleaseTask(this->GetFriend()->GetGameObjectPtr());
	this->mFriend.reset();

	taskManager->ReleaseTask(this->GetParty()->GetGameObjectPtr());
	this->mParty.reset();

	taskManager->ReleaseTask(this->GetSkill()->GetGameObjectPtr());
	this->mSkill.reset();
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

	if (false == Handle_LoadSkillTree_Request(packetSession, mToken.GetCharacterID()))
	{
		return false;
	}

	this->GetFriend()->NotifyConnectToFriend();

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

	if (false == GetFriend()->IsValid())
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
	GetCharacter()->GetEqipmentComponent().LoadEqipment(enterPacket.mutable_eqipment());
	enterPacket.set_money(GetInventory()->GetMoney());
	enterPacket.mutable_transform()->CopyFrom(PacketUtils::ToSTransform(character->GetTransform()));
	enterPacket.set_error(false);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, enterPacket);
	playerState->Send(sendBuffer);

	world->PushCharacterIDandRemoteID(this->mToken.GetCharacterID(), character->GetCharacterData().name(), this->GetGameObjectID());
	world->VisibleAreaInit(playerState);
}

void GameRemotePlayer::LeaveRemotePlayer()
{
	this->GetFriend()->NotifyDisConnectToFriend();
	this->GetCharacter()->SetLoadCharacter(false);
	this->GetInventory()->SetLoadInventory(false);
}

bool GameRemotePlayer::LeaveComplete()
{
	if (true == GetCharacter()->IsValid())
	{
		return false;
	}

	if (true == GetInventory()->IsValid())
	{
		return false;
	}

	if (true == GetFriend()->IsValid())
	{
		return false;
	}

	return true;
}
