#include "pch.h"
#include "World.h"

World::World(const SessionManagerRef& inSessionManager, const WCHAR* inName) : GameObject(inSessionManager, inName), mPlayersCount(0)
{
}

World::~World()
{
}

void World::Initialization()
{
	mPlayersCount = 0;
}

void World::Destroy()
{
	mPlayers.clear();
}

void World::Tick()
{
}

void World::Enter(PlayerStatePtr inPlayerState)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (remotePlayer)
	{
		return;
	}

	static int64 remoteID = 1;

	remotePlayer = std::make_shared<RemotePlayer>(remoteID);
	auto result = mPlayers.insert(std::make_pair(remoteID, inPlayerState));

	if (false == result.second)
	{
		inPlayerState->Disconnect(L"Do not create remote player");
		return;
	}

	Protocol::S2C_EnterGameServer enterPacket;
	enterPacket.set_remote_id(remoteID);
	enterPacket.set_error(result.second);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, enterPacket);
	packetSession->Send(sendBuffer);

	mPlayersCount++;
	remoteID++;

	for (auto& player : mPlayers)
	{
		if (player.second != inPlayerState)
		{
			AppearCharacter(player.second, inPlayerState);
			AppearCharacter(inPlayerState, player.second);
		}
	}

}

void World::Leave(PlayerStatePtr inPlayerState)
{
	RemotePlayerPtr& remotePlayer = inPlayerState->GetRemotePlayer();
	if (remotePlayer)
	{
		return;
	}

	const int64 remoteID = remotePlayer->mID;
	size_t result = mPlayers.erase(remoteID);

	int32 error = -1;
	if (result)
	{
		error = 0;
	}

	Protocol::S2C_LeaveGameServer leavePacket;
	leavePacket.set_error(error);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, leavePacket);
	packetSession->Send(sendBuffer);


	for (auto& player : mPlayers)
	{
		if (player.second != inPlayerState)
		{
			DisAppearCharacter(player.second, inPlayerState);
			DisAppearCharacter(inPlayerState, player.second);
		}
	}

	mPlayersCount--;
}

void World::Broadcast(SendBufferPtr sendBuffer)
{
	for (auto& player : mPlayers)
	{
		if (player.second)
		{
			player.second->Send(sendBuffer);
		}
	}
}

void World::AppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState)
{
	RemotePlayerPtr& appearRemotePlayer = inAppearPlayerState->GetRemotePlayer();
	if (nullptr == appearRemotePlayer)
	{
		return;
	}

	Protocol::S2C_AppearCharacter appearPacket;
	Protocol::SCharacterData* characterData = appearPacket.mutable_character_data();
	*characterData = appearRemotePlayer->mCharacterData;
	appearPacket.set_remote_id(appearRemotePlayer->mID);

	if (nullptr == inTargetPlayerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inTargetPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, appearPacket);
	packetSession->Send(sendBuffer);

}

void World::DisAppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState)
{
	RemotePlayerPtr& disappearRemotePlayer = inAppearPlayerState->GetRemotePlayer();
	if (nullptr == disappearRemotePlayer)
	{
		return;
	}

	Protocol::S2C_DisAppearCharacter disappearPacket;
	disappearPacket.set_remote_id(disappearRemotePlayer->mID);

	if (nullptr == inTargetPlayerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inTargetPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disappearPacket);
	packetSession->Send(sendBuffer);

}

void World::MoveDestination(PlayerStatePtr inPlayerState, Protocol::C2S_MovementCharacter inPakcet)
{
	RemotePlayerPtr remotePlayer = inPlayerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return;
	}

	Protocol::STransform	transform = inPakcet.transform();
	const int64				remoteID = remotePlayer->mID;
	const int64				timestamp = inPakcet.timestamp();

	Protocol::S2C_MovementCharacter newMovementPacket;
	Protocol::STransform* newTransform = newMovementPacket.mutable_transform();
	*newTransform = transform;
	newMovementPacket.set_remoteid(remoteID);
	newMovementPacket.set_timestamp(timestamp);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, newMovementPacket);
	Broadcast(sendBuffer);
}
