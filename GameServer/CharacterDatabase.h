#pragma once

bool Handle_LoadCharacter_Request(PacketSessionPtr& inSession, const int32 characterID);
bool Handle_LoadCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_Update_Experience_Request(PacketSessionPtr& inSession, const int32& characterID, const int32& inLevel, const int32& inExperience);
bool Handle_Update_Experience_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);