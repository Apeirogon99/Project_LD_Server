#pragma once

//CustomHandler
bool Handle_Singin_Requset(PacketSessionPtr& inSession, Protocol::C2S_Singin& inPacket);
bool Handle_Singin_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_Singup_Requset(PacketSessionPtr& inSession, Protocol::C2S_Singup& inPacket);
bool Handle_Singup_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_EmailVerified_Requset(PacketSessionPtr& inSession, Protocol::C2S_EmailVerified& inPacket);
bool Handle_EmailVerified_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_LoadCharacters_Requset(PacketSessionPtr& inSession, Protocol::C2S_LoadCharacters& inPacket);
bool Handle_LoadCharacters_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_CreateCharacter_Requset(PacketSessionPtr& inSession, Protocol::C2S_CreateCharacter& inPacket);
bool Handle_CreateCharacter_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_UpdateAppearance_Requset(PacketSessionPtr& inSession, Protocol::C2S_UpdateAppearance& inPacket);
bool Handle_UpdateAppearance_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_DeleteCharacter_Requset(PacketSessionPtr& inSession, Protocol::C2S_DeleteCharacter& inPacket);
bool Handle_DeleteCharacter_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_UpdateNickName_Requset(PacketSessionPtr& inSession, Protocol::C2S_UpdateNickName& inPacket);
bool Handle_UpdateNickName_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset);

class IdentitiyDatabaseHandler
{
public:
	static void AddADOWork(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset, ADOCallBack inCallback)
	{
		IdentityDatabasePtr identityDatabase = std::static_pointer_cast<IdentityDatabase>(inSession->GetSessionManager()->GetService()->GetDatabaseManager());
		identityDatabase->GetAsync().AddWork(inSession, inCommand, inRecordset, inCallback);
	}
};

