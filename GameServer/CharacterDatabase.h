#pragma once

bool Handle_LoadCharacter_Request(PacketSessionPtr& inSession, const int32 characterID);
bool Handle_LoadCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);