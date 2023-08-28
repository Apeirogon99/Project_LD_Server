#include "pch.h"
#include "LoginRoom.h"

LoginRoom::LoginRoom() : GameObject(L"LoginRoom")
{
	
}

LoginRoom::~LoginRoom()
{
	
}

void LoginRoom::OnInitialization()
{
	
}

void LoginRoom::OnDestroy()
{
	
}

void LoginRoom::OnTick(const int64 inDeltaTime)
{
}

bool LoginRoom::IsValid()
{
	return (this->GetOwner().lock() != nullptr);
}

void LoginRoom::Signin(PlayerStatePtr inPlayerState, Protocol::C2S_Singin inPacket)
{
	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	const std::string userName = inPacket.user_id();
	const std::string userPassword = inPacket.user_password();

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_Singin_Requset(packetSession, userName, userPassword);
}

void LoginRoom::Signup(PlayerStatePtr inPlayerState, Protocol::C2S_Singup inPacket)
{
	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	std::string tempEmail = inPacket.user_email();
	size_t find = tempEmail.find('@');

	std::string userID = inPacket.user_id().c_str();
	std::string userPS = inPacket.user_password().c_str();
	std::string local = tempEmail.substr(0, find).c_str();
	std::string domain = tempEmail.substr(find + 1, tempEmail.length()).c_str();

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_Singup_Requset(packetSession, userID, userPS, local, domain);
}

void LoginRoom::EmailVerified(PlayerStatePtr inPlayerState, Protocol::C2S_EmailVerified inPacket)
{
	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return;
	}

	ADOVariant global_id	= identityManager->GetGlobalID();
	ADOVariant verifyCode	= atoi(inPacket.verified_code().c_str());

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_EmailVerified_Requset(packetSession, global_id, verifyCode);
}

void LoginRoom::LoadServerRequest(PlayerStatePtr inPlayerState, Protocol::C2S_LoadServer inPacket)
{
	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return;
	}

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	Handle_Select_Character_Request(packetSession, identityManager->GetGlobalID());
}

void LoginRoom::SelectServer(PlayerStatePtr inPlayerState, Protocol::C2S_SelectServer inPacket)
{
	LoginWorldPtr world = std::static_pointer_cast<LoginWorld>(this->GetOwner().lock());
	if (nullptr == world)
	{
		return;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(inPlayerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return;
	}

	const int32 serverID = inPacket.server_id();


	identityManager->SetServerID(serverID);


	Protocol::S2C_SelectServer selectServerPacket;
	selectServerPacket.set_error(true);

	PacketSessionPtr packetSession = std::static_pointer_cast<PacketSession>(inPlayerState);
	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(packetSession, selectServerPacket);
	packetSession->Send(sendBuffer);
}
