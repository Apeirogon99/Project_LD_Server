#pragma once

bool Handle_Singin_Requset(PacketSessionPtr& inSession, const std::string& inUserName, const std::string& inUserPassword);
bool Handle_Singup_Requset(PacketSessionPtr& inSession, const std::string& inUserName, const std::string& inUserPassword, const std::string& inLocal, const std::string& inDomain);
bool Handle_EmailVerified_Requset(PacketSessionPtr& inSession, const int32& inGlobalID, const int32& inVeriftyCode);
bool Handle_Select_Character_Request(PacketSessionPtr& inSession, const int32 inGlobalID);

bool Handle_Singin_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_Singup_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_EmailVerified_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_Select_Character_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);