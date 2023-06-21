#pragma once

//==========================//
//		  Inventory			//
//==========================//

bool Handle_LoadInventory_Requset(PacketSessionPtr& inSession, const int32 characterID);
bool Handle_LoadInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_InsertInventory_Requset(PacketSessionPtr& inSession, const int32 inCharacterID, const int32 inItemCode, const Protocol::SVector2D& inInventoryPosition, const int32 inInventoryRotation);
bool Handle_InsertInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_UpdateInventory_Requset(PacketSessionPtr& inSession, const int32 inCharacterID, const int32 inItemCode, const Protocol::SVector2D& inOldInventoryPosition, const Protocol::SVector2D& inNewInventoryPosition, const int32 inNewRotation);
bool Handle_UpdateInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

bool Handle_DeleteInventory_Requset(PacketSessionPtr& inSession, const int32 inCharacterID, const int32 inItemCode, const Protocol::SVector2D& inInventoryPosition);
bool Handle_DeleteInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);

//==========================//
//		  Eqipment			//
//==========================//
bool Handle_ReplaceEqipment_Requset(PacketSessionPtr& inSession, AItemPtr inInsertInventoryItem, AItemPtr inInsertEqipmentItem, Protocol::ECharacterPart inPart);
bool Handle_ReplaceEqipment_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset);