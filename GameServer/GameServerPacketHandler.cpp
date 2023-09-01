#include "pch.h"
#include "GameServerPacketHandler.h"

using namespace std;

bool Handle_C2S_EnterGameServer(PacketSessionPtr& session, Protocol::C2S_EnterGameServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if(nullptr == gameState)
	{
		return false;
	}

	GameTaskPtr task = gameState->GetGameTask();
	if (nullptr == task)
	{
		return false;
	}

	GameWorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}
	
	const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &GameWorld::Enter, playerState, pkt);
	return true;
}

bool Handle_C2S_LeaveGameServer(PacketSessionPtr& session, Protocol::C2S_LeaveGameServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return false;
	}

	GameTaskPtr task = gameState->GetGameTask();
	if (nullptr == task)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	session->Disconnect(L"Handle_C2S_LeaveGameServer");
	
	/*const int64 serviceTimeStamp = gameState->GetServiceTimeStamp();
	world->PushTask(serviceTimeStamp, &World::Leave, playerState);*/
	return true;
}

bool Handle_C2S_Tick(PacketSessionPtr& session, Protocol::C2S_Tick& pkt)
{
	return true;
}

bool Handle_C2S_MovementCharacter(PacketSessionPtr& session, Protocol::C2S_MovementCharacter& pkt)
{

	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return false;
	}

	remotePlayer->GetCharacter()->PushTask(pkt.timestamp(), &PlayerCharacter::MovementCharacter, pkt);
	return true;
}

bool Handle_C2S_PlayAnimation(PacketSessionPtr& session, Protocol::C2S_PlayAnimation& pkt)
{
	return false;
}

bool Handle_C2S_PlayerAutoAttack(PacketSessionPtr& session, Protocol::C2S_PlayerAutoAttack& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return false;
	}


	character->PushTask(pkt.timestamp(), &PlayerCharacter::AutoAttack, pkt.object_id());
	return true;
}

bool Handle_C2S_Chat(PacketSessionPtr& session, Protocol::C2S_Chat& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return false;
	}

	GameTaskPtr task = gameState->GetGameTask();
	if (nullptr == task)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	world->PushTask(pkt.timestamp(), &GameWorld::WorldChat, playerState, pkt.timestamp(), pkt.message());
	return true;
}

bool Handle_C2S_LoadFriendList(PacketSessionPtr& session, Protocol::C2S_LoadFriendList& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	FriendPtr friends = remotePlayer->GetFriend();
	if (nullptr == friends)
	{
		return false;
	}

	friends->PushTask(pkt.timestamp(), &Friend::LoadFriendList, pkt.list_type());
	return true;
}

bool Handle_C2S_RequestFriend(PacketSessionPtr& session, Protocol::C2S_RequestFriend& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	FriendPtr friends = remotePlayer->GetFriend();
	if (nullptr == friends)
	{
		return false;
	}

	friends->PushTask(pkt.timestamp(), &Friend::RequestFriend, pkt.nick_name(), pkt.action());
	return true;
}

bool Handle_C2S_BlockFriend(PacketSessionPtr& session, Protocol::C2S_BlockFriend& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	FriendPtr friends = remotePlayer->GetFriend();
	if (nullptr == friends)
	{
		return false;
	}

	friends->PushTask(pkt.timestamp(), &Friend::BlockFriend, pkt.nick_name(), pkt.action());
	return true;
}

bool Handle_C2S_CreateParty(PacketSessionPtr& session, Protocol::C2S_CreateParty& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return false;
	}

	party->PushTask(pkt.timestamp(), &Party::CreateParty);
	return true;
}

bool Handle_C2S_RequestEnterParty(PacketSessionPtr& session, Protocol::C2S_RequestEnterParty& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return false;
	}

	party->PushTask(pkt.timestamp(), &Party::RequestEnterParty, pkt.nick_name());
	return true;
}

bool Handle_C2S_RequestLeaveParty(PacketSessionPtr& session, Protocol::C2S_RequestLeaveParty& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return false;
	}

	party->PushTask(pkt.timestamp(), &Party::RequestLeaveParty, pkt.remote_id());
	return true;
}

bool Handle_C2S_RequestLeaderParty(PacketSessionPtr& session, Protocol::C2S_RequestLeaderParty& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return false;
	}

	party->PushTask(pkt.timestamp(), &Party::RequestLeaderParty, pkt.remote_id());
	return true;
}

bool Handle_C2S_ResponeParty(PacketSessionPtr& session, Protocol::C2S_ResponeParty& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return false;
	}

	party->PushTask(pkt.timestamp(), &Party::ResponseEnterParty, pkt.remote_id(), pkt.action());
	return true;
}

bool Handle_C2S_InteractiveObject(PacketSessionPtr& session, Protocol::C2S_InteractiveObject& pkt)
{
	return true;
}

bool Handle_C2S_LoadInventory(PacketSessionPtr& session, Protocol::C2S_LoadInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	remotePlayer->GetInventory()->PushTask(pkt.timestamp(), &Inventory::LoadItemToInventory, pkt);
	return true;
}

bool Handle_C2S_InsertInventory(PacketSessionPtr& session, Protocol::C2S_InsertInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const Protocol::SItem&	item = pkt.item();
	const int64				timestmap = pkt.timestamp();

	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::InsertItemToInventory, item.object_id(), item.item_code(), FVector(item.world_position().x(), item.world_position().y(), item.world_position().z()), item.inven_position(), item.rotation(), item.amount());

	return true;
}

bool Handle_C2S_UpdateInventory(PacketSessionPtr& session, Protocol::C2S_UpdateInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::UpdateItemToInventory, pkt);

	return true;
}

bool Handle_C2S_DeleteInventory(PacketSessionPtr& session, Protocol::C2S_DeleteInventory& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::DeleteItemToInventory, pkt);
	return true;
}

bool Handle_C2S_ReplaceEqipment(PacketSessionPtr& session, Protocol::C2S_ReplaceEqipment& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	const int64 timestmap = pkt.timestamp();
	remotePlayer->GetInventory()->PushTask(timestmap, &Inventory::ReplcaeItemToEqipment, pkt);
	return true;
}

bool Handle_C2S_UpdateSkillTree(PacketSessionPtr& session, Protocol::C2S_UpdateSkillTree& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	SkillTreePtr skill = remotePlayer->GetSkillTree();
	if (nullptr == skill)
	{
		return false;
	}

	skill->PushTask(pkt.timestamp(), &SkillTree::UpdateSkillTree, pkt.skill_id(), pkt.skill_count());
	return true;
}

bool Handle_C2S_SetUseKeyAction(PacketSessionPtr& session, Protocol::C2S_SetUseKeyAction& pkt)
{
	return true;
}

bool Handle_C2S_PressedUseKeyAction(PacketSessionPtr& session, Protocol::C2S_PressedUseKeyAction& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}
	const int64& timestamp = pkt.timestamp();
	remotePlayer->PushTask(timestamp, &GameRemotePlayer::PressKey, pkt.key_id());

	return true;
}

bool Handle_C2S_ReleaseUseKeyAction(PacketSessionPtr& session, Protocol::C2S_ReleaseUseKeyAction& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(session);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}
	remotePlayer->PushTask(pkt.timestamp(), &GameRemotePlayer::ReleaseKey, pkt.key_id());

	return true;
}
