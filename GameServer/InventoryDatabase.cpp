#include "pch.h"
#include "InventoryDatabase.h"

bool Handle_InsertInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_InsertInventory& inPacket)
{
	return false;
}

bool Handle_InsertInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	return false;
}

bool Handle_UpdateInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_UpdateInventory& inPacket)
{
	return false;
}

bool Handle_UpdateInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	return false;
}

bool Handle_DeleteInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_DeleteInventory& inPacket)
{
	return false;
}

bool Handle_DeleteInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	return false;
}
