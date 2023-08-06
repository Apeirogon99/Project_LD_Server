#pragma once

bool Handle_ConnectLoadFriendList_Request(PacketSessionPtr& inSession, const int64& inCharacterID);
bool Handle_ConnectLoadFriendList_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_DisConnectLoadFriendList_Request(PacketSessionPtr& inSession, const int64& inCharacterID);
bool Handle_DisConnectLoadFriendList_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_LoadFriendList_Request(PacketSessionPtr& inSession, const int64& inCharacterID, const int32& inListType);
bool Handle_LoadFriendList_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_RequestFriend_Request(PacketSessionPtr& inSession, const int64& inUserCharacterID, const std::string& inFriendName, const int32& inAction);
bool Handle_RequestFriend_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_BlockFriend_Request(PacketSessionPtr& inSession, const int64& inUserCharacterID, const std::string& inFriendName, const int32& inAction);
bool Handle_BlockFriend_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_FindFriend_Request(PacketSessionPtr& inSession, const std::string& inFriendName);
bool Handle_FindFriend_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);