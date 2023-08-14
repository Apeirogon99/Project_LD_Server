#include "pch.h"
#include "Party.h"

Party::Party() : GameObject(L"Party"), mLoad(0), mLeaderPlayer(0)
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
	return this->mLoad;
}

void Party::SetPartyLoad(const bool& inLoad)
{
	this->mLoad = inLoad;
}

void Party::CreateParty()
{
	int32 error = 0;
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int64& remoteID = remotePlayer->GetGameObjectID();

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

	{
		Protocol::S2C_CreateParty createPartyPacket;
		createPartyPacket.set_error(error);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, createPartyPacket);
		remoteClient->Send(sendBuffer);
	}

	if (static_cast<int32>(EDCommonErrorType::SUCCESS) != error)
	{
		return;
	}

	this->PushPartyPlayer(remoteID, character);
	this->LeaderParty(remoteID);
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

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	GameRemotePlayerPtr otherRemotePlayer = nullptr;
	if (false == world->IsValidCharacter(inPlayerName, otherRemotePlayer))
	{
		error = static_cast<int32>(EDCommonErrorType::NOT_CONNECT_PLAYER);
	}

	PartyPtr otherParty = nullptr;
	if (otherRemotePlayer)
	{
		otherParty = otherRemotePlayer->GetParty();
		if (true == otherParty->IsParty())
		{
			error = static_cast<int32>(EDCommonErrorType::ALREADY_PART_OF_PARTY);
		}

		if (true == otherParty->IsReservationParty(otherRemotePlayer->GetGameObjectID()))
		{
			error = static_cast<int32>(EDCommonErrorType::ALREADY_RESERVATION_PARTY);
		}
	}

	{
		Protocol::S2C_RequestEnterParty partyPacket;
		partyPacket.set_error(error);
		partyPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, partyPacket);
		remoteClient->Send(sendBuffer);
	}

	if (static_cast<int32>(EDCommonErrorType::SUCCESS) != error)
	{
		return;
	}

	party->PushReservationParty(otherRemotePlayer->GetGameObjectID());

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

void Party::RequestLeaveParty(const int64 inLeaveRemoteID)
{
	int32 cause = 0;
	int32 error = 0;

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
			cause = 0;

			this->BroadCastLeavePartyWithKick(inLeaveRemoteID);

		}
		else if (remotePlayerID == mLeaderPlayer && remotePlayerID == inLeaveRemoteID)
		{
			cause = 2;

			this->ReleasePartyPlayer(inLeaveRemoteID);

			if (mPartyPlayers.size())
			{
				GameRemotePlayerPtr newLeaderRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(mPartyPlayers.begin()->second.lock()->GetOwner().lock());
				if (newLeaderRemotePlayer)
				{
					this->BroadCastLeavePartyWithLeader(inLeaveRemoteID, newLeaderRemotePlayer->GetGameObjectID());
				}
			}

			mPartyPlayers.clear();
			mLeaderPlayer = 0;

		}
		else
		{
			cause = 3;

			this->BroadCastLeaveParty(inLeaveRemoteID);

			mPartyPlayers.clear();
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
	int32 error = 0;
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

	if (party->GetLeaderRemoteID() != remotePlayerID)
	{
		error = error = static_cast<int32>(EDCommonErrorType::NOT_LEADER_PARTY);
	}

	Protocol::S2C_RequestLeaderParty leaderPartyPacket;
	leaderPartyPacket.set_error(error);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leaderPartyPacket);
	remoteClient->Send(sendBuffer);

	if (static_cast<int32>(EDCommonErrorType::SUCCESS) != error)
	{
		return;
	}

	this->BroadCastLeaderParty(inLeaderRemoteID);

}

void Party::ResponseEnterParty(const int64 inResponeRemoteID, const int32 inAction)
{
	int32 error = 0;

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}
	const int64& remoteID = remotePlayer->GetGameObjectID();

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return;
	}
	const Protocol::SCharacterData& characterData = character->GetCharacterData();

	RemoteClientPtr remoteClient = remotePlayer->GetRemoteClient().lock();
	if (nullptr == remoteClient)
	{
		return;
	}

	PartyPtr party = remotePlayer->GetParty();
	if (nullptr == party)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	GameRemotePlayerPtr otherRemotePlayer = nullptr;	//요청을 보낸사람
	if (false == world->IsValidPlayer(inResponeRemoteID, otherRemotePlayer))
	{
		error = static_cast<int32>(EDCommonErrorType::NOT_CONNECT_PLAYER);
	}

	PartyPtr otherParty = nullptr;
	if (otherRemotePlayer)
	{
		otherParty = otherRemotePlayer->GetParty();
		if (false == otherParty->IsParty())
		{
			error = static_cast<int32>(EDCommonErrorType::INVALID_PART_OF_PARTY);
		}

		if (true == party->IsParty())
		{
			error = static_cast<int32>(EDCommonErrorType::ALREADY_PART_OF_PARTY);
		}

		if (true == otherParty->IsFull())
		{
			error = static_cast<int32>(EDCommonErrorType::ALREADY_FULL_PARTY);
		}

		if (false == otherParty->IsReservationParty(remoteID))
		{
			error = static_cast<int32>(EDCommonErrorType::NOT_RESERVATION_PARTY);
		}

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

	if (static_cast<int32>(EDCommonErrorType::SUCCESS) != error)
	{
		return;
	}

	if (inAction == 2)
	{
		otherParty->BroadCastEnterParty(remoteID, character);

		party->LeaderParty(otherParty->GetLeaderRemoteID());
		party->SetPartyPlayers(otherParty->GetPartyPlayers());

		Protocol::S2C_LoadParty loadPartyPacket;
		party->LoadParty(loadPartyPacket);
		loadPartyPacket.set_error(error);
		loadPartyPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, loadPartyPacket);
		remoteClient->Send(sendBuffer);
	}
	otherParty->ReleaseReservationParty(remoteID);

	RemoteClientPtr otherRemoteClient = otherRemotePlayer->GetRemoteClient().lock();
	if (nullptr == otherRemoteClient)
	{
		return;
	}

	Protocol::S2C_NotifyParty notifyPacket;
	notifyPacket.set_nick_name(characterData.name());
	notifyPacket.set_action(inAction);
	notifyPacket.set_timestamp(world->GetWorldTime());

	SendBufferPtr NotyfyPacketSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, notifyPacket);
	otherRemoteClient->Send(NotyfyPacketSendBuffer);
		
	
}

void Party::RequestPartyChat(PlayerStatePtr inPlayerState, const int64 inWorldTime, std::string inMessage)
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
	for (auto player : mPartyPlayers)
	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(player.second.lock()->GetOwner().lock());
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

	inLoadPartyPacket.set_leader_id(this->mLeaderPlayer);

	for (auto player : mPartyPlayers)
	{
		GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(player.second.lock()->GetOwner().lock());
		if (nullptr == remotePlayer)
		{
			return;
		}
		const int64 remoteID = remotePlayer->GetGameObjectID();
		const Protocol::SCharacterData& characterData = player.second.lock()->GetCharacterData();

		inLoadPartyPacket.add_remote_id(remoteID);
		inLoadPartyPacket.add_nick_name(characterData.name());
		inLoadPartyPacket.add_level(characterData.level());
		inLoadPartyPacket.add_character_class(characterData.character_class());
	}
}

void Party::EnterParty(const int64& inRemoteID)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	RemoteClientPtr remoteClient = std::static_pointer_cast<RemoteClient>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	auto enterPlayer = mPartyPlayers.find(inRemoteID);
	if (enterPlayer == mPartyPlayers.end())
	{
		return;
	}
	const Protocol::SCharacterData& characterData = enterPlayer->second.lock()->GetCharacterData();

	Protocol::S2C_EnterPartyPlayer enterPartyPacket;
	enterPartyPacket.set_remote_id(inRemoteID);
	enterPartyPacket.set_leader_id(this->mLeaderPlayer);
	enterPartyPacket.set_nick_name(characterData.name());
	enterPartyPacket.set_level(characterData.level());
	enterPartyPacket.set_character_class(characterData.character_class());

	SendBufferPtr enterPacketSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, enterPartyPacket);
	PartyBroadCast(enterPacketSendBuffer);
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

	auto enterPlayer = mPartyPlayers.find(inRemoteID);
	if (enterPlayer != mPartyPlayers.end())
	{
		return;
	}

	Protocol::S2C_LeavePartyPlayer leavePartyPlayer;
	leavePartyPlayer.set_remote_id(inRemoteID);

	SendBufferPtr leavePartyPlayerSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePartyPlayer);
	remoteClient->Send(leavePartyPlayerSendBuffer);
}

void Party::LeaderParty(const int64& inRemoteID)
{
	this->mLeaderPlayer = inRemoteID;
}

void Party::BroadCastEnterParty(const int64& inRemoteID, PlayerCharacterPtr inPlayerCharacter)
{
	auto oldPlayer = mPartyPlayers.begin();
	for (oldPlayer; oldPlayer != mPartyPlayers.end(); oldPlayer++)
	{
		GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer->second.lock()->GetOwner().lock());
		if (nullptr == oldRemotePlayer)
		{
			return;
		}

		if (inRemoteID == oldRemotePlayer->GetGameObjectID())
		{
			continue;
		}

		PartyPtr party = oldRemotePlayer->GetParty();
		if (nullptr == party)
		{
			return;
		}

		party->PushPartyPlayer(inRemoteID, inPlayerCharacter);
		party->EnterParty(inRemoteID);
	}
}

void Party::BroadCastLeaveParty(const int64& inRemoteID)
{

	auto oldPlayer = mPartyPlayers.begin();
	for (oldPlayer; oldPlayer != mPartyPlayers.end(); oldPlayer++)
	{
		GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer->second.lock()->GetOwner().lock());
		if (nullptr == oldRemotePlayer)
		{
			return;
		}

		if (inRemoteID == oldRemotePlayer->GetGameObjectID())
		{
			continue;
		}

		PartyPtr party = oldRemotePlayer->GetParty();
		if (nullptr == party)
		{
			return;
		}

		party->ReleasePartyPlayer(inRemoteID);
		party->LeaveParty(inRemoteID);
	}
}

void Party::BroadCastLeavePartyWithKick(const int64& inRemoteID)
{
	auto find = mPartyPlayers.find(inRemoteID);
	if (find == mPartyPlayers.end())
	{
		return;
	}

	GameRemotePlayerPtr romtePlayer = std::static_pointer_cast<GameRemotePlayer>(find->second.lock()->GetOwner().lock());
	if (nullptr == romtePlayer)
	{
		return;
	}

	RemoteClientPtr remoteClient = std::static_pointer_cast<RemoteClient>(romtePlayer->GetRemoteClient().lock());
	if (nullptr == remoteClient)
	{
		return;
	}

	{
		Protocol::S2C_RequestLeaveParty leavePartyPacket;
		leavePartyPacket.set_error(0);
		leavePartyPacket.set_cause(1);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, leavePartyPacket);
		remoteClient->Send(sendBuffer);
	}
	this->ReleasePartyPlayer(inRemoteID);

	auto oldPlayer = mPartyPlayers.begin();
	for (oldPlayer; oldPlayer != mPartyPlayers.end(); oldPlayer++)
	{
		GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer->second.lock()->GetOwner().lock());
		if (nullptr == oldRemotePlayer)
		{
			return;
		}

		PartyPtr party = oldRemotePlayer->GetParty();
		if (nullptr == party)
		{
			return;
		}

		party->ReleasePartyPlayer(inRemoteID);
		party->LeaveParty(inRemoteID);
	}
}

void Party::BroadCastLeavePartyWithLeader(const int64& inRemoteID, const int64& inLeaderRemoteID)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	auto oldPlayer = mPartyPlayers.begin();
	for (oldPlayer; oldPlayer != mPartyPlayers.end(); oldPlayer++)
	{
		GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer->second.lock()->GetOwner().lock());
		if (nullptr == oldRemotePlayer)
		{
			return;
		}

		if (remotePlayer->GetGameObjectID() == oldRemotePlayer->GetGameObjectID())
		{
			continue;
		}

		RemoteClientPtr oldRemoteClient = oldRemotePlayer->GetRemoteClient().lock();
		if (nullptr == oldRemoteClient)
		{
			return;
		}

		PartyPtr party = oldRemotePlayer->GetParty();
		if (nullptr == party)
		{
			return;
		}
		//party->LeaveParty(inRemoteID);
		party->LeaderParty(inLeaderRemoteID);
		party->ReleasePartyPlayer(inRemoteID);

		Protocol::S2C_LoadParty loadPartyPacket;
		loadPartyPacket.set_error(0);
		party->LoadParty(loadPartyPacket);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, loadPartyPacket);
		oldRemoteClient->Send(sendBuffer);
	}
}

void Party::BroadCastLeaderParty(const int64& inRemoteID)
{
	auto oldPlayer = mPartyPlayers.begin();
	for (oldPlayer; oldPlayer != mPartyPlayers.end(); oldPlayer++)
	{
		GameRemotePlayerPtr oldRemotePlayer = std::static_pointer_cast<GameRemotePlayer>(oldPlayer->second.lock()->GetOwner().lock());
		if (nullptr == oldRemotePlayer)
		{
			return;
		}

		RemoteClientPtr oldRemoteClient = oldRemotePlayer->GetRemoteClient().lock();
		if (nullptr == oldRemoteClient)
		{
			return;
		}

		PartyPtr party = oldRemotePlayer->GetParty();
		if (nullptr == party)
		{
			return;
		}
		party->LeaderParty(inRemoteID);

		Protocol::S2C_LoadParty loadPartyPacket;
		loadPartyPacket.set_error(0);
		party->LoadParty(loadPartyPacket);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, loadPartyPacket);
		oldRemoteClient->Send(sendBuffer);
	}
}

bool Party::PushPartyPlayer(const int64& inRemoteID, PlayerCharacterPtr inPlayerCharacter)
{
	auto find = mPartyPlayers.find(inRemoteID);
	if (find != mPartyPlayers.end())
	{
		return false;
	}

	std::pair<int64, PlayerCharacterPtr> newPartyPlayer = std::make_pair(inRemoteID, inPlayerCharacter);
	mPartyPlayers.insert(newPartyPlayer);

	return true;
}

bool Party::ReleasePartyPlayer(const int64& inRemoteID)
{
	auto find = mPartyPlayers.find(inRemoteID);
	if (find == mPartyPlayers.end())
	{
		return false;
	}

	mPartyPlayers.erase(inRemoteID);

	return true;
}

bool Party::IsPartyPlayer(const int64& inRemoteID)
{
	auto find = mPartyPlayers.find(inRemoteID);
	if (find == mPartyPlayers.end())
	{
		return false;
	}

	return true;
}

bool Party::PushReservationParty(const int64& inRemoteID)
{
	auto find = mReservationPlayers.find(inRemoteID);
	if (find != mReservationPlayers.end())
	{
		return false;
	}

	mReservationPlayers.insert(inRemoteID);

	return true;
}

bool Party::ReleaseReservationParty(const int64& inRemoteID)
{
	auto find = mReservationPlayers.find(inRemoteID);
	if (find == mReservationPlayers.end())
	{
		return false;
	}

	mReservationPlayers.erase(inRemoteID);

	return true;
}

bool Party::IsReservationParty(const int64& inRemoteID)
{
	auto find = mReservationPlayers.find(inRemoteID);
	if (find == mReservationPlayers.end())
	{
		return false;
	}

	return true;
}

void Party::SetPartyPlayers(const std::map<int64, PlayerCharacterRef>& inPartyPlayers)
{
	mPartyPlayers = inPartyPlayers;
}

bool Party::IsParty() const
{
	return mPartyPlayers.size();
}

bool Party::IsFull() const
{
	return mPartyPlayers.size() == 4;
}

const std::map<int64, PlayerCharacterRef> Party::GetPartyPlayers() const
{
	return mPartyPlayers;
}

const std::set<int64> Party::GetReservationPlayers() const
{
	return mReservationPlayers;
}

const int64& Party::GetLeaderRemoteID() const
{
	return mLeaderPlayer;
}
