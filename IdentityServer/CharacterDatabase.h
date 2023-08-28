#pragma once

bool Handle_LoadCharacters_Requset(PacketSessionPtr& inSession, const int32& inGlobalID, const int32& inServerID);
bool Handle_CreateCharacter_Requset(PacketSessionPtr& inSession, const std::string& inName, const int32& inClass, const int32& inRace, const int32& inSeat, const int32& inGlobalID, const int32& inServerID, const int32& inSkinColor, const int32& inHairID, const int32& inHairColor, const int32& inEyeColor, const int32& inEyebrowColor);
bool Handle_DeleteCharacter_Requset(PacketSessionPtr& inSession, const int32& inGlobalID, const int32& inServerID, const int32& inCharacterID);

bool Handle_LoadCharacters_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_CreateCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);
bool Handle_DeleteCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);