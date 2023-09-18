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

	mSkillTree = std::make_shared<SkillTree>();
	mSkillTree->SetOwner(owner);
	taskManager->PushTask(this->GetSkillTree()->GetGameObjectPtr());

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

	taskManager->ReleaseTask(this->GetSkillTree()->GetGameObjectPtr());
	this->mSkillTree.reset();
}

void GameRemotePlayer::OnTick(const int64 inDeltaTime)
{

}

bool GameRemotePlayer::IsValid()
{
	return true;
}

bool GameRemotePlayer::LoadRemotePlayer(const Token& inToken)
{
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
	this->GetParty()->SetPartyLoad(true);

	std::map<int32, BindAction> keyActions;
	keyActions.insert(std::make_pair(static_cast<int32>('Q'), BindAction(EBindActionType::Action_Skill, static_cast<int32>(ESkillID::Skill_Warrior_Parrying))));				 
	keyActions.insert(std::make_pair(static_cast<int32>('W'), BindAction(EBindActionType::Action_Skill, static_cast<int32>(ESkillID::Skill_Warrior_ShieldBash))));			 
	keyActions.insert(std::make_pair(static_cast<int32>('E'), BindAction(EBindActionType::Action_Skill, static_cast<int32>(ESkillID::Skill_Warrior_Buff))));					 
	keyActions.insert(std::make_pair(static_cast<int32>('R'), BindAction(EBindActionType::Action_Skill, static_cast<int32>(ESkillID::Skill_Warrior_SwordBlow))));
	keyActions.insert(std::make_pair(static_cast<int32>(VK_LCONTROL), BindAction(EBindActionType::Action_Skill, static_cast<int32>(ESkillID::Skill_Warrior_Dash))));
	this->mKeyboardComponet.LoadBindKeyAndAction(std::static_pointer_cast<GameRemotePlayer>(shared_from_this()), keyActions);

	return true;
}

void GameRemotePlayer::SetWorld(GameWorldRef inWorld)
{
	mWorld = inWorld;
	mPlayerCharacter->SetWorld(inWorld);
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

	this->GetParty()->RequestLeaveParty(this->GetGameObjectID());
	this->GetParty()->SetPartyLoad(false);

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

	if (true == GetParty()->IsValid())
	{
		return false;
	}

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(this->GetRemoteClient().lock());
	if (nullptr == playerState)
	{
		return false;
	}

	mTaskManagerRef.lock()->ReleaseTask(this->GetGameObjectPtr());

	Protocol::S2C_LeaveGameServer leavePacket;
	leavePacket.set_error(true);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePacket);
	playerState->Send(sendBuffer);

	return true;
}

void GameRemotePlayer::PressKey(int32 inKey)
{
	this->mKeyboardComponet.PressedKey(inKey);
}

void GameRemotePlayer::ReleaseKey(int32 inKey)
{
	this->mKeyboardComponet.ReleaseKey(inKey);
}
