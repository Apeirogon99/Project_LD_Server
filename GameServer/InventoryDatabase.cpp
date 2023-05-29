#include "pch.h"
#include "InventoryDatabase.h"

bool Handle_LoadCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	return true;
}

bool Handle_LoadInventory_Requset(PacketSessionPtr& inSession, const int32 characterID)
{
	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	//TODO:
	ADOVariant character_id = characterID;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_inventory_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_LoadInventory_Response);
	return true;
}

bool Handle_LoadInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return false;
	}

	WorldPtr world = gameState->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	int32 ret = inCommand.GetReturnParam();
	if (ret == 0)
	{
		if (!inRecordset.IsOpen())
		{
			inRecordset.Open(inCommand, inConnection);
		}

		while (!inRecordset.IsEof())
		{
			int32		item_code	= inRecordset.GetFieldItem(L"item_code");
			float		world_pos_x	= inRecordset.GetFieldItem(L"world_pos_x");
			float		world_pos_y = inRecordset.GetFieldItem(L"world_pos_y");
			float		world_pos_z = inRecordset.GetFieldItem(L"world_pos_z");
			int32		inven_pos_x = inRecordset.GetFieldItem(L"inven_pos_x");
			int32		inven_pos_y = inRecordset.GetFieldItem(L"inven_pos_y");
			int32		rotation	= inRecordset.GetFieldItem(L"rotation");

			const int64 objectID = world->GetNextGameObjectID();
			remotePlayer->GetInventory()->PushItem(objectID, item_code, world_pos_x, world_pos_y, world_pos_z, inven_pos_x, inven_pos_y, rotation);
			inRecordset.MoveNext();
		}
	}

	Protocol::S2C_LoadInventory loadInventoryPacket;

	remotePlayer->GetInventory()->LoadItem(loadInventoryPacket);

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, loadInventoryPacket);
	inSession->Send(sendBuffer);

	remotePlayer->GetInventory()->CheckInventory();

	return true;
}

bool Handle_InsertInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_InsertInventory& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
	if (nullptr == gameState)
	{
		return false;
	}

	WorldPtr world = gameState->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	Protocol::SItem* item	= inPacket.mutable_item();
	ADOVariant character_id = 0;	//TODO:
	ADOVariant item_code	= item->item_code();

	Protocol::SVector* worldPosition = item->mutable_world_position();
	ADOVariant world_pos_x	= worldPosition->x();
	ADOVariant world_pos_y	= worldPosition->y();
	ADOVariant world_pos_z	= worldPosition->z();

	Protocol::SVector2D* inventoryPosition = item->mutable_inven_position();
	ADOVariant inven_pos_x	= inventoryPosition->x();
	ADOVariant inven_pos_y	= inventoryPosition->y();

	ADOVariant rotation		= item->rotation();
	
	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.insert_inventory_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id"	, character_id);
	command.SetInputParam(L"@item_code"		, item_code);
	command.SetInputParam(L"@world_pos_x"	, world_pos_x);
	command.SetInputParam(L"@world_pos_y"	, world_pos_y);
	command.SetInputParam(L"@world_pos_z"	, world_pos_z);
	command.SetInputParam(L"@inven_pos_x"	, inven_pos_x);
	command.SetInputParam(L"@inven_pos_y"	, inven_pos_y);
	command.SetInputParam(L"@rotation"		, rotation);

	{
		bool pushResult = remotePlayer->GetInventory()->PushItem(item);

		Protocol::S2C_InsertInventory InsertInventoryPacket;
		InsertInventoryPacket.set_remote_id(remotePlayer->GetRemoteID());
		InsertInventoryPacket.set_object_id(item->object_id());
		InsertInventoryPacket.set_error(pushResult);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, InsertInventoryPacket);
		world->Broadcast(sendBuffer);
	}

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_InsertInventory_Response);
	return true;
}

bool Handle_InsertInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	int32 ret = inCommand.GetReturnParam();
	if (ret != 0)
	{
		//ERROR:
	}

	if (false == remotePlayer->GetInventory()->CheckInventory())
	{
		//TODO:
	}

	return true;
}

bool Handle_UpdateInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_UpdateInventory& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	const Protocol::SItem& item = inPacket.item();
	ADOVariant character_id = 0;	//TODO:
	ADOVariant item_code = 1;

	AItem tempItem;
	remotePlayer->GetInventory()->FindItem(item.object_id(), tempItem);
	ADOVariant old_inven_pos_x = tempItem.mInventoryPositionX;
	ADOVariant old_inven_pos_y = tempItem.mInventoryPositionY;

	const Protocol::SVector2D& inventoryPosition = item.inven_position();
	ADOVariant new_inven_pos_x = inventoryPosition.x();
	ADOVariant new_inven_pos_y = inventoryPosition.y();

	ADOVariant new_rotation = item.rotation();

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.update_inventory_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@item_code", item_code);
	command.SetInputParam(L"@old_inven_pos_x", old_inven_pos_x);
	command.SetInputParam(L"@old_inven_pos_y", old_inven_pos_y);
	command.SetInputParam(L"@new_inven_pos_x", new_inven_pos_x);
	command.SetInputParam(L"@new_inven_pos_y", new_inven_pos_y);
	command.SetInputParam(L"@new_rotation", new_rotation);

	{
		bool result = remotePlayer->GetInventory()->UpdateItem(item);

		Protocol::S2C_UpdateInventory updateInventoryPacket;
		updateInventoryPacket.set_error(result);
		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, updateInventoryPacket);
		inSession->Send(sendBuffer);

		remotePlayer->GetInventory()->CheckInventory();
	}

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);
	

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_InsertInventory_Response);
	return true;
}

bool Handle_UpdateInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	int32 ret = inCommand.GetReturnParam();
	if (ret != 0)
	{
		//ERROR:
	}

	if (false == remotePlayer->GetInventory()->CheckInventory())
	{
		//TODO:
	}

	return true;
}

bool Handle_DeleteInventory_Requset(PacketSessionPtr& inSession, Protocol::C2S_DeleteInventory& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	const Protocol::SItem& item = inPacket.item();
	ADOVariant character_id = 0;	//TODO:
	ADOVariant item_code = item.item_code();

	const Protocol::SVector2D& inventoryPosition = item.inven_position();
	ADOVariant inven_pos_x = inventoryPosition.x();
	ADOVariant inven_pos_y = inventoryPosition.y();

	remotePlayer->GetInventory()->UpdateItem(item);

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.delete_inventory_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@item_code", item_code);
	command.SetInputParam(L"@inven_pos_x", inven_pos_x);
	command.SetInputParam(L"@inven_pos_y", inven_pos_y);

	{
		bool result = remotePlayer->GetInventory()->DeleteItem(item);

		Protocol::S2C_DeleteInventory deleteInventoryPacket;
		deleteInventoryPacket.set_remote_id(remotePlayer->GetRemoteID());
		*deleteInventoryPacket.mutable_item() = item;
		deleteInventoryPacket.set_error(result);

		GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
		if (nullptr == gameState)
		{
			return false;
		}

		WorldPtr world = gameState->GetWorld();
		if (nullptr == world)
		{
			return false;
		}

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, deleteInventoryPacket);
		world->Broadcast(sendBuffer);
	}

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_InsertInventory_Response);
	return true;
}

bool Handle_DeleteInventory_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	int32 ret = inCommand.GetReturnParam();
	if (ret != 0)
	{
		//ERROR:
	}

	if (false == remotePlayer->GetInventory()->CheckInventory())
	{
		//TODO:
	}

	return true;
}
