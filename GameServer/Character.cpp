#include "pch.h"
#include "Character.h"

Character::Character(const RemotePlayerRef& inReomtePlayer) : Pawn(L"Character"), mRemotePlayer(inReomtePlayer), mCharacterID(-1), mLastMovementTimeStamp(0)
{

}

Character::~Character()
{

}

void Character::Initialization()
{
	mLastMovementTimeStamp = 0;
}

void Character::Destroy()
{
}

void Character::Tick()
{
}

bool Character::IsValid()
{
	return true;
}

void Character::AppearActor(PlayerStatePtr inAppearPlayerState)
{
	RemotePlayerPtr targetRemotePlayer = mRemotePlayer.lock();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	Viewers& viewers = targetRemotePlayer->GetViewers();
	if (viewers.find(inAppearPlayerState) != viewers.end())
	{
		return;
	}
	targetRemotePlayer->GetViewers().insert(inAppearPlayerState);
	inAppearPlayerState->GetMonitors().insert(targetRemotePlayer);

	RemotePlayerPtr appearRemotePlayer = inAppearPlayerState->GetRemotePlayer();
	if (nullptr == appearRemotePlayer)
	{
		return;
	}

	Protocol::S2C_AppearCharacter appearPacket;
	appearPacket.set_remote_id(targetRemotePlayer->GetGameObjectID());
	appearPacket.set_timestamp(this->mLastMovementTimeStamp);
	
	Protocol::SVector* oldMovementLocation = appearPacket.mutable_old_location();
	oldMovementLocation->CopyFrom(this->mOldLocation);

	Protocol::SVector* newMovementLocation = appearPacket.mutable_new_location();
	newMovementLocation->CopyFrom(this->GetLocation());

	Protocol::SCharacterData* characterData = appearPacket.mutable_character_data();
	characterData->CopyFrom(this->GetCharacterData());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inAppearPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, appearPacket);
	inAppearPlayerState->Send(sendBuffer);
}

void Character::DisAppearActor(PlayerStatePtr inDisappearPlayerState)
{
	RemotePlayerPtr targetRemotePlayer = mRemotePlayer.lock();
	if (nullptr == targetRemotePlayer)
	{
		return;
	}

	Viewers& viewers = targetRemotePlayer->GetViewers();
	if (viewers.find(inDisappearPlayerState) == viewers.end())
	{
		return;
	}
	targetRemotePlayer->GetViewers().erase(inDisappearPlayerState);

	RemotePlayerPtr disappearRemotePlayer = inDisappearPlayerState->GetRemotePlayer();
	if (nullptr == disappearRemotePlayer)
	{
		return;
	}

	Protocol::S2C_DisAppearCharacter disappearPacket;
	disappearPacket.set_remote_id(targetRemotePlayer->GetGameObjectID());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inDisappearPlayerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, disappearPacket);
	inDisappearPlayerState->Send(sendBuffer);
}

void Character::MoveDestination(Protocol::C2S_MovementCharacter inPakcet)
{
	RemotePlayerPtr remotePlayer = mRemotePlayer.lock();
	if (nullptr == remotePlayer)
	{
		return;
	}
	
	const Protocol::SVector&	oldLocation = inPakcet.old_location();
	const Protocol::SVector&	newLocation	= inPakcet.new_location();
	const int64					remoteID	= remotePlayer->GetGameObjectID();
	const int64					timestamp	= inPakcet.timestamp();

	this->mLastMovementTimeStamp = timestamp;
	this->mOldLocation.CopyFrom(oldLocation);
	this->SetLocation(newLocation);

	Protocol::S2C_MovementCharacter newMovementPacket;
	Protocol::SVector* oldMovementLocation = newMovementPacket.mutable_old_location();
	oldMovementLocation->CopyFrom(oldLocation);

	Protocol::SVector* newMovementLocation = newMovementPacket.mutable_new_location();
	newMovementLocation->CopyFrom(newLocation);

	newMovementPacket.set_remote_id(remoteID);
	newMovementPacket.set_timestamp(timestamp);

	PlayerStatePtr playerState = remotePlayer->GetPlayerState().lock();
	if (nullptr == playerState)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(playerState);
	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(packetSession, newMovementPacket);
	playerState->BrodcastViewers(sendBuffer);
}

void Character::SetCharacterData(Protocol::SCharacterData inCharacterData)
{
	mCharacterData.CopyFrom(inCharacterData);
}
