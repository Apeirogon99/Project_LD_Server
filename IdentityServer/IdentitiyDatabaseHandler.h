#pragma once

//CustomHandler
void Handle_Singin_Requset(PacketSessionPtr& session, Protocol::C2S_Singin& pkt);
void Handle_Singin_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet);

void Handle_Singup_Requset(PacketSessionPtr& session, Protocol::C2S_Singup& pkt);
void Handle_Singup_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet);

void Handle_EmailVerified_Requset(PacketSessionPtr& session, Protocol::C2S_EmailVerified& pkt);
void Handle_EmailVerified_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet);

void Handle_LoadCharacters_Requset(PacketSessionPtr& session, Protocol::C2S_LoadCharacters& pkt);
void Handle_LoadCharacters_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet);

void Handle_CreateCharacter_Requset(PacketSessionPtr& session, Protocol::C2S_CreateCharacter& pkt);
void Handle_CreateCharacter_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet);

void Handle_UpdateAppearance_Requset(PacketSessionPtr& session, Protocol::C2S_UpdateAppearance& pkt);
void Handle_UpdateAppearance_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet);

void Handle_DeleteCharacter_Requset(PacketSessionPtr& session, Protocol::C2S_DeleteCharacter& pkt);
void Handle_DeleteCharacter_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet);

void Handle_UpdateNickName_Requset(PacketSessionPtr& session, Protocol::C2S_UpdateNickName& pkt);
void Handle_UpdateNickName_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet);

class IdentitiyDatabaseHandler
{
public:
	static void AddADOWork(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset, ADOCallBack inCallback)
	{
		IdentityDatabasePtr identityDatabase = std::static_pointer_cast<IdentityDatabase>(inSession->GetSessionManager()->GetService()->GetDatabaseManager());
		identityDatabase->GetAsync().AddWork(inSession, inCommand, inRecordset, inCallback);
	}
};

