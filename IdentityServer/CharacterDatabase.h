#pragma once

bool Handle_LoadCharacters_Requset(PacketSessionPtr& inSession, Protocol::C2S_LoadCharacters& inPacket);
bool Handle_CreateCharacter_Requset(PacketSessionPtr& inSession, Protocol::C2S_CreateCharacter& inPacket);
bool Handle_DeleteCharacter_Requset(PacketSessionPtr& inSession, Protocol::C2S_DeleteCharacter& inPacket);

bool Handle_LoadCharacters_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_CreateCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_DeleteCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);