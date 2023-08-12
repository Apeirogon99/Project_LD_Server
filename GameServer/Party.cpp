#include "pch.h"
#include "Party.h"

Party::Party() : GameObject(L"Party"), mLeaderPlayer(0)
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
		error = static_cast<int32>(EDCommonErrorType::ALREADY_PART_OF_PARTY);
	}

	Protocol::S2C_CreateParty createPartyPacket;
	createPartyPacket.set_error(error);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, createPartyPacket);
	remoteClient->Send(sendBuffer);

	std::pair<int64, PlayerCharacterPtr> player = std::make_pair(remotePlayer->GetGameObjectID(), character);
	mPlayers.insert(player);

	mLeaderPlayer = remotePlayer->GetGameObjectID();
}

void Party::RequestEnterParty(const std::string inPlayerName)
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

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	GameRemotePlayerPtr otherRemotePlayer;
	if (false == world->IsValidCharacter(inPlayerName, otherRemotePlayer))
	{
		error = static_cast<int32>(EDCommonErrorType::NOT_CONNECT_PLAYER);
	}

	if (otherRemotePlayer)
	{
		PartyPtr party = otherRemotePlayer->GetParty();
		if (true == party->IsParty())
		{
			error = static_cast<int32>(EDCommonErrorType::ALREADY_PART_OF_PARTY);
		}
	}

	{
		Protocol::S2C_RequestEnterParty partyPacket;
		partyPacket.set_error(error);
		partyPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, partyPacket);
		remoteClient->Send(sendBuffer);
	}

	if (0 == error)
	{
		PlayerCharacterPtr character = remotePlayer->GetCharacter();
		if (nullptr == character)
		{
			return;
		}
		const Protocol::SCharacterData& characterData = character->GetCharacterData();

		RemoteClientPtr otherRemoteClient = otherRemotePlayer->GetRemoteClient().lock();
		if (nullptr == otherRemoteClient)
		{
			return;
		}

		Protocol::S2C_RequestParty requestPartyPacket;
		requestPartyPacket.set_remote_id(remotePlayer->GetGameObjectID());
		requestPartyPacket.set_nick_name(characterData.name());
		requestPartyPacket.set_level(characterData.level());
		requestPartyPacket.set_character_class(characterData.character_class());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, requestPartyPacket);
		otherRemoteClient->Send(sendBuffer);
	}
}

void Party::RequestLeaveParty(const int64 inLeaveRemoteID)
{
	int32 error = 0;
	int32 cause = 0;

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int64& remotePlayerID = remotePlayer->GetGameObjectID();

	RemoteClientPtr remoteClient = remotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (false == party->IsParty())
	{
		error = error = static_cast<int32>(EDCommonErrorType::INVALID_PART_OF_PARTY);
	}

	if (0 == error)
	{

		if (remotePlayerID == mLeaderPlayer && remotePlayerID != inLeaveRemoteID)
		{
			cause = 1;
			for (auto oldPlayer : mPlayers)
			{
				GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer.second->GetOwner().lock());
				if (nullptr == oldRemotePlayer)
				{
					return;
				}

				if (oldRemotePlayer->GetGameObjectID() == inLeaveRemoteID)
				{
					RemoteClientPtr oldRemoteClient = std::static_pointer_cast<RemoteClient>(oldRemotePlayer->GetRemoteClient().lock());
					if (nullptr == oldRemoteClient)
					{
						return;
					}

					Protocol::S2C_RequestLeaveParty leavePartyPacket;
					leavePartyPacket.set_error(error);
					leavePartyPacket.set_cause(cause);

					SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePartyPacket);
					oldRemoteClient->Send(sendBuffer);
				}
				else if (oldRemotePlayer->GetGameObjectID() != remotePlayerID)
				{
					oldRemotePlayer->GetParty()->LeaveParty(inLeaveRemoteID);
				}

			}
			this->LeaveParty(inLeaveRemoteID);
			return;
		}
		else if (remotePlayerID == mLeaderPlayer && remotePlayerID == inLeaveRemoteID)
		{
			cause = 2;

			GameRemotePlayerPtr newLeaderRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(mPlayers.begin()->second->GetOwner().lock());
			if (nullptr == newLeaderRemotePlayer)
			{
				return;
			}

			for (auto oldPlayer : mPlayers)
			{
				GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer.second->GetOwner().lock());
				if (nullptr == oldRemotePlayer)
				{
					return;
				}

				if (oldRemotePlayer->GetGameObjectID() != inLeaveRemoteID)
				{
					oldRemotePlayer->GetParty()->LeaveParty(inLeaveRemoteID);
					oldRemotePlayer->GetParty()->RequestLeaderParty(newLeaderRemotePlayer->GetGameObjectID());
				}
			}
			mPlayers.clear();
			mLeaderPlayer = 0;
		}
		else
		{
			cause = 3;

			for (auto oldPlayer : mPlayers)
			{
				GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer.second->GetOwner().lock());
				if (nullptr == oldRemotePlayer)
				{
					return;
				}

				if (oldRemotePlayer->GetGameObjectID() != inLeaveRemoteID)
				{
					oldRemotePlayer->GetParty()->LeaveParty(remotePlayer->GetGameObjectID());
				}
			}
			mPlayers.clear();
			mLeaderPlayer = 0;
		}
	}

	Protocol::S2C_RequestLeaveParty leavePartyPacket;
	leavePartyPacket.set_error(error);
	leavePartyPacket.set_cause(cause);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePartyPacket);
	remoteClient->Send(sendBuffer);
}

void Party::RequestLeaderParty(const int64 inLeaderRemoteID)
{
}

void Party::ResponseEnterParty(const int64 inResponeRemoteID, const int32 inAction)
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

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (inAction == 2)
	{
		Protocol::S2C_LoadParty loadPartyPacket;
		GameRemotePlayerPtr otherRemotePlayer;
		if (false == world->IsValidPlayer(inResponeRemoteID, otherRemotePlayer))
		{
			error = static_cast<int32>(EDCommonErrorType::NOT_CONNECT_PLAYER);
		}

		if (otherRemotePlayer)
		{
			PartyPtr party = otherRemotePlayer->GetParty();
			if (false == party->IsParty())
			{
				error = static_cast<int32>(EDCommonErrorType::INVALID_PART_OF_PARTY);
			}

			if (true == party->IsFull())
			{
				error = static_cast<int32>(EDCommonErrorType::ALREADY_FULL_PARTY);
			}

			party->EnterParty(remotePlayer->GetGameObjectID(), remotePlayer);

			party->LoadParty(loadPartyPacket);
		}
		loadPartyPacket.set_error(error);
		loadPartyPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, loadPartyPacket);
		remoteClient->Send(sendBuffer);
	}
	else if (inAction == 3)
	{

	}

	{
		Protocol::S2C_ResponeParty responePacket;
		responePacket.set_error(error);
		responePacket.set_action(inAction);
		responePacket.set_remote_id(inResponeRemoteID);
		responePacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, responePacket);
		remoteClient->Send(sendBuffer);
	}


	if (0 == error)
	{
		PlayerCharacterPtr character = remotePlayer->GetCharacter();
		if (nullptr == character)
		{
			return;
		}

		GameRemotePlayerPtr otherRemotePlayer;
		if (true == world->IsValidPlayer(inResponeRemoteID, otherRemotePlayer))
		{
			RemoteClientPtr otherRemoteClient = otherRemotePlayer->GetRemoteClient().lock();
			if (nullptr == otherRemoteClient)
			{
				return;
			}

			Protocol::S2C_NotifyParty notifyPacket;
			notifyPacket.set_nick_name(character->GetCharacterData().name());
			notifyPacket.set_action(inAction);
			notifyPacket.set_timestamp(world->GetWorldTime());

			SendBufferPtr NotyfyPacketSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, notifyPacket);
			otherRemoteClient->Send(NotyfyPacketSendBuffer);
		}
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

void Party::LoadParty(Protocol::S2C_LoadParty& inLoadPartyPacket)
{
	for (auto player : mPlayers)
	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(player.second->GetOwner().lock());
		if (nullptr == remotePlayer)
		{
			return;
		}
		const Protocol::SCharacterData& characterData = player.second->GetCharacterData();

		inLoadPartyPacket.add_remote_id(remotePlayer->GetGameObjectID());
		inLoadPartyPacket.add_is_leader(static_cast<int32>(this->mLeaderPlayer == remotePlayer->GetGameObjectID()));
		inLoadPartyPacket.add_nick_name(characterData.name());
		inLoadPartyPacket.level(characterData.level());
		inLoadPartyPacket.add_character_class(characterData.character_class());
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

	Protocol::S2C_LoadParty loadParty;
	loadParty.set_error(0);
	for (auto oldPlayer : mPlayers)
	{
		PlayerCharacterPtr oldCharacter = oldPlayer.second;
		const Protocol::SCharacterData& oldCharacterData = character->GetCharacterData();
		loadParty.add_remote_id(oldCharacter->GetOwner().lock()->GetGameObjectID());
		loadParty.add_nick_name(characterData.name());
		loadParty.add_level(characterData.level());
		loadParty.add_character_class(characterData.character_class());
	}

	RemoteClientPtr remoteClient = inRemotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	SendBufferPtr ResponePartySendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, loadParty);
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

	if (inRemoteID == remotePlayer->GetGameObjectID())
	{
		mPlayers.clear();
		mLeaderPlayer = 0;
	}
	else
	{
		auto find = mPlayers.find(inRemoteID);
		if (find == mPlayers.end())
		{
			return;
		}
		mPlayers.erase(inRemoteID);
	}

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
