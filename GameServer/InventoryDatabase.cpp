#include "pch.h"
#include "InventoryDatabase.h"

bool Handle_LoadInventory_Requset(PacketSessionPtr& inSession, const int32 characterID)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
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

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	Inventoryptr inventory = remotePlayer->GetInventory();
	if (nullptr == inventory)
	{
		return false;
	}

	WorldPtr world = remotePlayer->GetWorld().lock();
	if (nullptr == world)
	{
		return false;
	}

	GameTaskPtr task = std::static_pointer_cast<GameTask>(world->GetTaskManagerRef().lock());
	if (nullptr == task)
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
			int32		inven_pos_x = inRecordset.GetFieldItem(L"inven_pos_x");
			int32		inven_pos_y = inRecordset.GetFieldItem(L"inven_pos_y");
			int32		rotation	= inRecordset.GetFieldItem(L"rotation");

			AItemPtr newItem = std::make_shared<AItem>();
			GameObjectPtr itemGameObject = newItem->GetGameObjectPtr();
			task->CreateGameObject(itemGameObject);
			newItem->Init(item_code, inven_pos_x, inven_pos_y, rotation);
			inventory->InsertItem(newItem);

			inRecordset.MoveNext();
		}
	}

	inventory->SetLoadInventory(true);
	remotePlayer->OnLoadComplete();

	return true;
}

bool Handle_InsertInventory_Requset(PacketSessionPtr& inSession, const int32 inCharacterID, const int32 inItemCode, const Protocol::SVector2D& inInventoryPosition, const int32 inInventoryRotation)
{

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inCharacterID;
	ADOVariant item_code	= inItemCode;

	const Protocol::SVector2D& inventoryPosition = inInventoryPosition;
	ADOVariant inven_pos_x	= inventoryPosition.x();
	ADOVariant inven_pos_y	= inventoryPosition.y();

	ADOVariant rotation		= inInventoryRotation;
	
	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.insert_inventory_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id"	, character_id);
	command.SetInputParam(L"@item_code"		, item_code);
	command.SetInputParam(L"@inven_pos_x"	, inven_pos_x);
	command.SetInputParam(L"@inven_pos_y"	, inven_pos_y);
	command.SetInputParam(L"@rotation"		, rotation);
	

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
	if (ret == ErrorToInt(EDCommonErrorType::FAILURE))
	{
		playerState->PlayerStateLog(L"InsertInventory Rollback");

		//데이터베이스에서 처리 불가능 판단
		//Inventory에 있는 아이템 제거
		//World에 아이템 추가

	}

	return true;
}

bool Handle_UpdateInventory_Requset(PacketSessionPtr& inSession, const int32 inCharacterID, const int32 inItemCode, const Protocol::SVector2D& inOldInventoryPosition, const Protocol::SVector2D& inNewInventoryPosition, const int32 inNewRotation)
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

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inCharacterID;
	ADOVariant item_code = inItemCode;

	ADOVariant old_inven_pos_x = inOldInventoryPosition.x();
	ADOVariant old_inven_pos_y = inOldInventoryPosition.y();

	ADOVariant new_inven_pos_x = inNewInventoryPosition.x();
	ADOVariant new_inven_pos_y = inNewInventoryPosition.y();

	ADOVariant new_rotation = inNewRotation;

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
	if (ret == ErrorToInt(EDCommonErrorType::FAILURE))
	{
		playerState->PlayerStateLog(L"UpdateInventory Rollback");

		//데이터베이스에서 처리 불가능 판단
		//Inventory에 있는 아이템 위치 돌아가기
	}

	return true;
}

bool Handle_DeleteInventory_Requset(PacketSessionPtr& inSession, const int32 inCharacterID, const int32 inItemCode, const Protocol::SVector2D& inInventoryPosition)
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

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inCharacterID;
	ADOVariant item_code = inItemCode;

	const Protocol::SVector2D& inventoryPosition = inInventoryPosition;
	ADOVariant inven_pos_x = inventoryPosition.x();
	ADOVariant inven_pos_y = inventoryPosition.y();

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.delete_inventory_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@item_code", item_code);
	command.SetInputParam(L"@inven_pos_x", inven_pos_x);
	command.SetInputParam(L"@inven_pos_y", inven_pos_y);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_DeleteInventory_Response);
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
	if (ret == ErrorToInt(EDCommonErrorType::FAILURE))
	{
		playerState->PlayerStateLog(L"DeleteInventory Rollback");

		//데이터베이스에서 처리 불가능 판단
		//World에 있는 아이템 제거
		//Inventory에 아이템 생성
	}

	return true;
}

bool Handle_ReplaceEqipment_Requset(PacketSessionPtr& inSession, AItemPtr inInsertInventoryItem, AItemPtr inInsertEqipmentItem, Protocol::ECharacterPart inPart)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);


	ADOVariant character_id			= remotePlayer->GetCharacter()->GetCharacterID();
	ADOVariant inven_item_code		= inInsertInventoryItem->GetItemCode();
	ADOVariant inven_pos_x			= inInsertInventoryItem->GetInventoryPosition().x();
	ADOVariant inven_pos_y			= inInsertInventoryItem->GetInventoryPosition().y();
	ADOVariant inven_rotation		= inInsertInventoryItem->GetInventoryRoation();

	ADOVariant eqipment_part		= static_cast<int32>(inPart);
	ADOVariant eqipment_item_code	= inInsertEqipmentItem->GetItemCode();
	ADOVariant eqipment_pos_x		= inInsertEqipmentItem->GetInventoryPosition().x();
	ADOVariant eqipment_pos_y		= inInsertEqipmentItem->GetInventoryPosition().y();

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.replace_eqipment_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@inven_item_code", inven_item_code);
	command.SetInputParam(L"@inven_pos_x", inven_pos_x);
	command.SetInputParam(L"@inven_pos_y", inven_pos_y);
	command.SetInputParam(L"@inven_rotation", inven_rotation);
	command.SetInputParam(L"@eqipment_part", eqipment_part);
	command.SetInputParam(L"@eqipment_item_code", eqipment_item_code);
	command.SetInputParam(L"@eqipment_pos_x", eqipment_pos_x);
	command.SetInputParam(L"@eqipment_pos_y", eqipment_pos_y);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);


	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_ReplaceEqipment_Response);
	return true;
}

bool Handle_ReplaceEqipment_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<PlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	int32 ret = inCommand.GetReturnParam();
	if (false == remotePlayer->GetInventory()->CheckInventory() || ret != 0)
	{
		remotePlayer->GetInventory()->RollBackItem();
	}

	return true;
}