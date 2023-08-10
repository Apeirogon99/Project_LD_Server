#pragma once

bool Handle_LoadSkillTree_Request(PacketSessionPtr& inSession, const int64& inCharacterID);
bool Handle_LoadSkillTree_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_UpdateSkillTree_Request(PacketSessionPtr& inSession, const int64& inCharacterID, const int32& inSkillID, const int32& inSkillCount);
bool Handle_UpdateSkillTree_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);