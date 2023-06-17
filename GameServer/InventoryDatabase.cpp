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

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	Inventoryptr inventory = remotePlayer->GetInventory();
	if (nullptr == inventory)
	{
		return false;
	}

	WorldPtr world = playerState->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	GameTaskPtr task = world->GetGameTask();
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

			inventory->CreateItem(item_code, inven_pos_x, inven_pos_y, rotation);
			inRecordset.MoveNext();
		}
	}

	inventory->SetLoad(true);

	remotePlayer->LoadComplete();

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

	GameTaskPtr task = gameState->GetGameTask();
	if (nullptr == task)
	{
		return false;
	}

	WorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	const Protocol::SItem& item	= inPacket.item();
	ADOVariant character_id = remotePlayer->GetCharacter()->GetCharacterID();
	ADOVariant item_code	= item.item_code();

	const Protocol::SVector& worldPosition = item.world_position();
	ADOVariant world_pos_x	= worldPosition.x();
	ADOVariant world_pos_y	= worldPosition.y();
	ADOVariant world_pos_z	= worldPosition.z();

	const Protocol::SVector2D& inventoryPosition = item.inven_position();
	ADOVariant inven_pos_x	= inventoryPosition.x();
	ADOVariant inven_pos_y	= inventoryPosition.y();

	ADOVariant rotation		= item.rotation();
	
	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.insert_inventory_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id"	, character_id);
	command.SetInputParam(L"@item_code"		, item_code);
	command.SetInputParam(L"@inven_pos_x"	, inven_pos_x);
	command.SetInputParam(L"@inven_pos_y"	, inven_pos_y);
	command.SetInputParam(L"@rotation"		, rotation);

	{
		GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
		if (nullptr == gameState)
		{
			return false;
		}

		GameTaskPtr task = gameState->GetGameTask();
		if (nullptr == task)
		{
			return false;
		}

		WorldPtr world = task->GetWorld();
		if (nullptr == world)
		{
			return false;
		}

		world->DestroyActor(item.object_id());

		AItemPtr newItem = std::make_shared<AItem>();
		newItem->Init(item);
		bool pushResult = remotePlayer->GetInventory()->InsertItem(newItem);

		Protocol::S2C_InsertInventory InsertInventoryPacket;
		InsertInventoryPacket.set_remote_id(remotePlayer->GetGameObjectID());
		InsertInventoryPacket.set_object_id(item.object_id());
		InsertInventoryPacket.set_error(pushResult);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, InsertInventoryPacket);
		playerState->BrodcastViewers(sendBuffer);
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
	if (false == remotePlayer->GetInventory()->CheckInventory() || ret != 0)
	{

		WorldPtr world = playerState->GetWorld();
		if (nullptr == world)
		{
			return false;
		}

		int32 code	= inCommand.GetParam(L"@item_code");
		int32 posx	= inCommand.GetParam(L"@inven_pos_x");
		int32 posy	= inCommand.GetParam(L"@inven_pos_y");

		AItemPtr findItem;
		bool find = remotePlayer->GetInventory()->FindItem(code, posx, posy, findItem);
		if (find == false)
		{
			return false;
		}

		Protocol::SItem deleteItem;
		deleteItem.set_object_id(findItem->GetGameObjectID());
		deleteItem.set_item_code(findItem->GetItemCode());
		deleteItem.mutable_world_position()->CopyFrom(findItem->GetLocation());
		deleteItem.mutable_inven_position()->CopyFrom(findItem->GetInventoryPosition());
		deleteItem.set_rotation(findItem->GetInventoryRoation());

		bool result = remotePlayer->GetInventory()->DeleteItem(findItem);

		Protocol::SRotator rotation;
		rotation.set_pitch(0);
		rotation.set_roll(0);
		rotation.set_yaw(0);

		AItemPtr newItem = std::static_pointer_cast<AItem>(world->CreateActor<AItem>(deleteItem.world_position(), rotation));
		newItem->Init(deleteItem);

		Protocol::S2C_DeleteInventory deleteInventoryPacket;
		deleteInventoryPacket.set_remote_id(remotePlayer->GetGameObjectID());
		deleteInventoryPacket.mutable_item()->CopyFrom(deleteItem);
		deleteInventoryPacket.set_error(result);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, deleteInventoryPacket);
		playerState->BrodcastViewers(sendBuffer);
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

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	const Protocol::SItem& item = inPacket.item();
	ADOVariant character_id = remotePlayer->GetCharacter()->GetCharacterID();
	ADOVariant item_code = item.item_code();

	AItemPtr findItem;
	bool find = remotePlayer->GetInventory()->FindItem(item.object_id(), findItem);
	if (nullptr == findItem)
	{
		return false;
	}

	ADOVariant old_inven_pos_x = findItem->GetInventoryPosition().x();
	ADOVariant old_inven_pos_y = findItem->GetInventoryPosition().y();

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
		bool result = remotePlayer->GetInventory()->UpdateItem(findItem);

		Protocol::S2C_UpdateInventory updateInventoryPacket;
		updateInventoryPacket.set_error(result);
		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, updateInventoryPacket);
		inSession->Send(sendBuffer);
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
	if (false == remotePlayer->GetInventory()->CheckInventory() || ret != 0)
	{
		remotePlayer->GetInventory()->RollBackItem();
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

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	const Protocol::SItem& item = inPacket.item();
	ADOVariant character_id = remotePlayer->GetCharacter()->GetCharacterID();
	ADOVariant item_code = item.item_code();

	const Protocol::SVector2D& inventoryPosition = item.inven_position();
	ADOVariant inven_pos_x = inventoryPosition.x();
	ADOVariant inven_pos_y = inventoryPosition.y();

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.delete_inventory_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@item_code", item_code);
	command.SetInputParam(L"@inven_pos_x", inven_pos_x);
	command.SetInputParam(L"@inven_pos_y", inven_pos_y);

	{
		GameStatePtr gameState = std::static_pointer_cast<GameState>(playerState->GetSessionManager());
		if (nullptr == gameState)
		{
			return false;
		}

		GameTaskPtr task = gameState->GetGameTask();
		if (nullptr == task)
		{
			return false;
		}

		WorldPtr world = task->GetWorld();
		if (nullptr == world)
		{
			return false;
		}

		AItemPtr findItem;
		bool find = remotePlayer->GetInventory()->FindItem(item.object_id(), findItem);
		if (false == find)
		{
			return false;
		}

		bool result = remotePlayer->GetInventory()->DeleteItem(findItem);

		Protocol::SRotator rotation;
		rotation.set_pitch(0);
		rotation.set_roll(0);
		rotation.set_yaw(0);

		AItemPtr newItem = std::static_pointer_cast<AItem>(world->CreateActor<AItem>(item.world_position(), rotation));
		newItem->Init(item);

		Protocol::S2C_DeleteInventory deleteInventoryPacket;
		deleteInventoryPacket.set_remote_id(remotePlayer->GetGameObjectID());
		deleteInventoryPacket.mutable_item()->CopyFrom(item);
		deleteInventoryPacket.set_error(result);

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, deleteInventoryPacket);
		playerState->BrodcastViewers(sendBuffer);
	}

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
	if (false == remotePlayer->GetInventory()->CheckInventory() || ret != 0)
	{
		remotePlayer->GetInventory()->RollBackItem();
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

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
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


	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_InsertInventory_Response);
	return true;
}

bool Handle_ReplaceEqipment_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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
	if (false == remotePlayer->GetInventory()->CheckInventory() || ret != 0)
	{
		remotePlayer->GetInventory()->RollBackItem();
	}

	return true;
}