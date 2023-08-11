#include "pch.h"
#include "Party.h"

Party::Party() : GameObject(L"Party")
{
}

Party::~Party()
{
}

void Party::OnInitialization()
{
}

void Party::OnDestroy()
{
}

void Party::OnTick(const int64 inDeltaTime)
{
}

bool Party::IsValid()
{
	return (this->GetOwner().lock() != nullptr);
}

void Party::CreateParty()
{
	int32 error = 0;
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	RemoteClientPtr remoteClient = remotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(remotePlayer->GetCharacter());
	if (nullptr == character)
	{
		return;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return;
	}

	if (true == party->IsParty())
	{
		error = -1;
	}

	Protocol::S2C_CreateParty createPartyPacket;
	createPartyPacket.set_error(error);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, createPartyPacket);
	remoteClient->Send(sendBuffer);

	std::pair<int64, PlayerCharacterPtr> player = std::make_pair(remotePlayer->GetGameObjectID(), character);
	mPlayers.insert(player);
}

void Party::RequestEnterParty(const std::string inPlayerName)
{
	int32 error = 0;

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	GameRemotePlayerPtr otherRemotePlayer;
	if (false == world->IsValidPlayer(inPlayerName, otherRemotePlayer))
	{
		return;
	}

	PartyPtr party = otherRemotePlayer->GetParty();
	if(true == party->IsParty())
	{
		error = -1;
	}

	RemoteClientPtr remoteClient = remotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	Protocol::S2C_RequestEnterParty partyPacket;
	partyPacket.set_error(error);
	partyPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, partyPacket);
	remoteClient->Send(sendBuffer);

	if (0 == error)
	{
		PlayerCharacterPtr character = remotePlayer->GetCharacter();
		if (nullptr == character)
		{
			return;
		}

		RemoteClientPtr otherRemoteClient = remotePlayer->GetRemoteClient().lock();
		if (nullptr == otherRemoteClient)
		{
			return;
		}

		Protocol::S2C_NotifyParty notifyPacket;
		notifyPacket.set_nick_name(character->GetCharacterData().name());
		notifyPacket.set_action(1);
		notifyPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, notifyPacket);
		otherRemoteClient->Send(sendBuffer);
	}
}

void Party::RequestLeaveParty(const std::string inPlayerName)
{
	int32 error = 0;

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	RemoteClientPtr remoteClient = remotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (false == party->IsParty())
	{
		error = -1;
	}

	for (auto oldPlayer : mPlayers)
	{
		GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer.second->GetOwner().lock());
		if (nullptr == oldRemotePlayer)
		{
			return;
		}

		if (remotePlayer->GetGameObjectID() != oldRemotePlayer->GetGameObjectID())
		{
			oldRemotePlayer->GetParty()->LeaveParty(remotePlayer->GetGameObjectID());
		}
	}
	mPlayers.clear();

	Protocol::S2C_RequestLeaveParty leavePartyPacket;
	leavePartyPacket.set_error(error);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePartyPacket);
	remoteClient->Send(sendBuffer);
}

void Party::RequestLeaderParty(const std::string inPlayerName)
{
}

void Party::ResponseEnterParty(const std::string inPlayerName, const int32 inAction)
{
	int32 error = 0;

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	GameRemotePlayerPtr otherRemotePlayer;
	if (false == world->IsValidPlayer(inPlayerName, otherRemotePlayer))
	{
		return;
	}

	PartyPtr party = otherRemotePlayer->GetParty();
	if (false == party->IsParty())
	{
		error = -1;
	}

	if (true == party->IsFull())
	{
		error = -1;
	}

	RemoteClientPtr remoteClient = remotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	Protocol::S2C_ResponeParty responePartyPacket;
	responePartyPacket.set_error(error);
	responePartyPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, responePartyPacket);
	remoteClient->Send(sendBuffer);

	if (0 == error)
	{
		PlayerCharacterPtr character = remotePlayer->GetCharacter();
		if (nullptr == character)
		{
			return;
		}

		party->EnterParty(remotePlayer->GetGameObjectID(), remotePlayer);

		RemoteClientPtr otherRemoteClient = remotePlayer->GetRemoteClient().lock();
		if (nullptr == otherRemoteClient)
		{
			return;
		}

		Protocol::S2C_NotifyParty notifyPacket;
		notifyPacket.set_nick_name(character->GetCharacterData().name());
		notifyPacket.set_action(1);
		notifyPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr NotyfyPacketSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, notifyPacket);
		otherRemoteClient->Send(NotyfyPacketSendBuffer);
	}
}

void Party::PartyChat(PlayerStatePtr inPlayerState, const int64 inWorldTime, std::string inMessage)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (false == party->IsParty())
	{
		return;
	}

	PlayerCharacterPtr playerCharacter = remotePlayer->GetCharacter();
	if (nullptr == playerCharacter)
	{
		return;
	}

	Protocol::S2C_Chat chatPacket;
	chatPacket.set_remote_id(remotePlayer->GetGameObjectID());
	chatPacket.set_name(playerCharacter->GetCharacterData().name());
	chatPacket.set_message(inMessage);
	chatPacket.set_chat_type(Protocol::Chat_Party);
	chatPacket.set_timestamp(inWorldTime);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, chatPacket);
	this->PartyBroadCast(sendBuffer);
}

void Party::PartyBroadCast(SendBufferPtr inSendBuffer)
{
	for (auto player : mPlayers)
	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(player.second->GetOwner().lock());
		if (nullptr == remotePlayer)
		{
			return;
		}

		RemoteClientPtr remoteClient = std::static_pointer_cast<RemoteClient>(remotePlayer->GetRemoteClient().lock());
		if (nullptr == remoteClient)
		{
			return;
		}

		remoteClient->Send(inSendBuffer);
	}
}

void Party::EnterParty(const int64& inRemoteID, GameRemotePlayerPtr inRemotePlayer)
{

	auto find = mPlayers.find(inRemoteID);
	if (find != mPlayers.end())
	{
		return;
	}

	PlayerCharacterPtr character = inRemotePlayer->GetCharacter();
	const Protocol::SCharacterData& characterData = character->GetCharacterData();

	Protocol::S2C_EnterPartyPlayer enterPartyPacket;
	enterPartyPacket.set_remote_id(inRemotePlayer->GetGameObjectID());
	enterPartyPacket.set_is_leader(0);
	enterPartyPacket.set_nick_name(characterData.name());
	enterPartyPacket.set_level(characterData.level());
	enterPartyPacket.set_character_class(characterData.character_class());

	SendBufferPtr EnterPacketSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, enterPartyPacket);
	PartyBroadCast(EnterPacketSendBuffer);

	Protocol::S2C_ResponeParty ResponeParty;
	ResponeParty.set_error(0);
	for (auto oldPlayer : mPlayers)
	{
		PlayerCharacterPtr oldCharacter = oldPlayer.second;
		const Protocol::SCharacterData& oldCharacterData = character->GetCharacterData();
		ResponeParty.add_remote_id(oldCharacter->GetOwner().lock()->GetGameObjectID());
		ResponeParty.add_nick_name(characterData.name());
		ResponeParty.add_level(characterData.level());
		ResponeParty.add_character_class(characterData.character_class());
	}

	RemoteClientPtr remoteClient = inRemotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	SendBufferPtr ResponePartySendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, ResponeParty);
	remoteClient->Send(ResponePartySendBuffer);
	
	std::pair<int64, PlayerCharacterPtr> newPlayer = std::make_pair(inRemoteID, inRemotePlayer->GetCharacter());
	mPlayers.insert(newPlayer);
}

void Party::LeaveParty(const int64& inRemoteID)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	RemoteClientPtr remoteClient = remotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	auto find = mPlayers.find(inRemoteID);
	if (find == mPlayers.end())
	{
		return;
	}
	mPlayers.erase(inRemoteID);

	Protocol::S2C_LeavePartyPlayer leavePartyPlayer;
	leavePartyPlayer.set_remote_id(inRemoteID);

	SendBufferPtr leavePartyPlayerSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePartyPlayer);
	remoteClient->Send(leavePartyPlayerSendBuffer);
}

bool Party::IsParty() const
{
	return mPlayers.size();
}

bool Party::IsFull() const
{
	return mPlayers.size() == 4;
}
