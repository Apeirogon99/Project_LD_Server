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

void Party::CreateParty(const int64 inRemoteID)
{
	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(this->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PlayerCharacterPtr character = std::static_pointer_cast<PlayerCharacter>(remotePlayer->GetCharacter());
	if (nullptr == character)
	{
		return;
	}

	if (remotePlayer->GetGameObjectID() != inRemoteID)
	{
		return;
	}

	auto findPlayer = mPlayers.find(inRemoteID);
	if (findPlayer != mPlayers.end())
	{
		return;
	}
	else
	{
		std::pair<int64, PlayerCharacterPtr> player = std::make_pair(inRemoteID, character);
		mPlayers.insert(player);
	}

}

void Party::RequestParty(const std::string inPlayerName)
{
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
		//�������� �ƴ�
		return;
	}

	PartyPtr party = otherRemotePlayer->GetParty();
	if(true == party->IsParty())
	{
		//��Ƽ�� �̹� ����
		return;
	}

	//������ �ȴٸ�
	//���� �÷��̾����� ������ ���ٰ�
	//�ٸ� �÷��̾����� ��Ƽ �ʴ븦 ������

}

void Party::ResponseParty(const std::string inPlayerName, const int32 inAction)
{
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
		return;
	}

	if (true == party->IsFull())
	{
		return;
	}

	//������ �ȴٸ�
	//���� �÷��̾����� ������ ���� ��Ƽ�� ���ӵ� �ο��� ������ ����
	//�ٸ� ��Ƽ �÷��̾����� ��Ƽ�� ���ߴٰ� ����
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

bool Party::IsParty() const
{
	return mPlayers.size();
}

bool Party::IsFull() const
{
	return mPlayers.size() == 4;
}
