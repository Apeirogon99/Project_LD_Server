#pragma once

bool Handle_Singin_Requset(PacketSessionPtr& inSession, Protocol::C2S_Singin& inPacket);
bool Handle_Singup_Requset(PacketSessionPtr& inSession, Protocol::C2S_Singup& inPacket);
bool Handle_EmailVerified_Requset(PacketSessionPtr& inSession, Protocol::C2S_EmailVerified& inPacket);

bool Handle_Singin_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_Singup_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_EmailVerified_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);