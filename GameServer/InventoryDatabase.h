#pragma once

//==========================//
//		  Inventory			//
//==========================//

bool Handle_LoadInventory_Requset(PacketSessionPtr& inSession, const int32 characterID);
bool Handle_LoadInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_InsertInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_InsertInventory& inPacket);
bool Handle_InsertInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_UpdateInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_UpdateInventory& inPacket);
bool Handle_UpdateInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_DeleteInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_DeleteInventory& inPacket);
bool Handle_DeleteInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

//==========================//
//		  Eqipment			//
//==========================//
bool Handle_InsertEqipment_Requset(PacketSessionPtr& inSession, Protocol::C2S_InsertEqipment& inPacket);
bool Handle_InsertEqipment_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_DeleteEqipment_Requset(PacketSessionPtr& inSession, Protocol::C2S_DeleteEqipment& inPacket);
bool Handle_DeleteEqipment_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);