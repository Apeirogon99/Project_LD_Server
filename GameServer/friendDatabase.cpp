#include "pch.h"
#include "friendDatabase.h"

bool Handle_ConnectLoadFriendList_Request(PacketSessionPtr& inSession, const int64& inCharacterID)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inCharacterID;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_friend_id_list_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_ConnectLoadFriendList_Response);
	return true;
}

bool Handle_ConnectLoadFriendList_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	FriendPtr friends = remotePlayer->GetFriend();
	if (nullptr == friends)
	{
		return false;
	}
	int64 playerCharacterID = inCommand.GetParam(L"@character_id");

	int32 error = inCommand.GetReturnParam();
	if (error == ErrorToInt(EDCommonErrorType::SUCCESS))
	{
		std::set<int64> firendIDs;
		while (!inRecordset.IsEof())
		{
			int64 friendID	= inRecordset.GetFieldItem(L"friend_character_id");
			firendIDs.insert(friendID);
			inRecordset.MoveNext();
		}
		friends->PushFriend(firendIDs);

		GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
		if (nullptr == world)
		{
			return false;
		}

		std::string playerName = remotePlayer->GetCharacter()->GetCharacterData().name();
		for (const int64& id : firendIDs)
		{
			GameRemotePlayerPtr friendRemotePlayer;
			if (true == world->IsValidCharacter(id, friendRemotePlayer))
			{
				FriendPtr otherFriend = friendRemotePlayer->GetFriend();
				otherFriend->DelegateOnlineFriend(playerCharacterID, playerName);
			}
		}
	}

	friends->SetLoadFriend(error == ErrorToInt(EDCommonErrorType::SUCCESS));
	remotePlayer->OnLoadComplete();
	return true;
}

bool Handle_DisConnectLoadFriendList_Request(PacketSessionPtr& inSession, const int64& inCharacterID)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inCharacterID;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_friend_id_list_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_DisConnectLoadFriendList_Response);
	return true;
}

bool Handle_DisConnectLoadFriendList_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	FriendPtr friends = remotePlayer->GetFriend();
	if (nullptr == friends)
	{
		return false;
	}
	int64 playerCharacterID = inCommand.GetParam(L"@character_id");

	int32 error = inCommand.GetReturnParam();
	if (error == ErrorToInt(EDCommonErrorType::SUCCESS))
	{
		std::set<int64> firendIDs;
		while (!inRecordset.IsEof())
		{
			int64 friendID = inRecordset.GetFieldItem(L"friend_character_id");
			firendIDs.insert(friendID);
			inRecordset.MoveNext();
		}
		friends->PushFriend(firendIDs);

		GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
		if (nullptr == world)
		{
			return false;
		}

		std::string playerName = remotePlayer->GetCharacter()->GetCharacterData().name();
		for (const int64& id : firendIDs)
		{
			GameRemotePlayerPtr friendRemotePlayer;
			if (true == world->IsValidCharacter(id, friendRemotePlayer))
			{
				FriendPtr otherFriend = friendRemotePlayer->GetFriend();
				otherFriend->DelegateOfflineFriend(playerCharacterID, playerName);
			}
		}
	}

	friends->SetLoadFriend(error != ErrorToInt(EDCommonErrorType::SUCCESS));
	remotePlayer->LeaveComplete();
	return true;
}

bool Handle_LoadFriendList_Request(PacketSessionPtr& inSession, const int64& inCharacterID, const int32& inListType)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inCharacterID;
	ADOVariant list_type	= inListType;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_friend_list_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@list_type", list_type);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_LoadFriendList_Response);
	return true;
}

bool Handle_LoadFriendList_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset) 
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

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}

	FriendPtr friends = remotePlayer->GetFriend();
	if (nullptr == friends)
	{
		return false;
	}

	int32 list_type = inCommand.GetParam(L"@list_type");

	Protocol::S2C_LoadFriendList loadFriendPacket;
	int32 error = inCommand.GetReturnParam();
	if (error == ErrorToInt(EDCommonErrorType::SUCCESS))
	{
		while (!inRecordset.IsEof())
		{
			Protocol::SFriend* newFriend = loadFriendPacket.add_friends();

			int32 friendID		= inRecordset.GetFieldItem(L"friend_character_id");
			std::string	name	= inRecordset.GetFieldItem(L"name");
			int32 classID		= inRecordset.GetFieldItem(L"character_calss_id");
			int32 level			= inRecordset.GetFieldItem(L"level");
			int32 locale		= inRecordset.GetFieldItem(L"locale");

			newFriend->set_nick_name(name);
			newFriend->set_character_class(static_cast<Protocol::ECharacterClass>(classID));
			newFriend->set_level(level);
			newFriend->set_locale(locale);

			if (list_type == 0)
			{
				newFriend->set_state((world->IsValidCharacter(friendID) == true) ? 1 : 3);
			}
			else
			{
				newFriend->set_state(0);
			}

			inRecordset.MoveNext();
		}
	}
	loadFriendPacket.set_list_type(list_type);
	loadFriendPacket.set_timestamp(remotePlayer->GetWorld().lock()->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, loadFriendPacket);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_RequestFriend_Request(PacketSessionPtr& inSession, const int64& inUserCharacterID, const std::string& inFriendName, const int32& inAction)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inUserCharacterID;
	ADOVariant friend_name = inFriendName.c_str();
	ADOVariant action = inAction;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.request_friend_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@friend_name", friend_name);
	command.SetInputParam(L"@action", action);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_RequestFriend_Response);
	return true;
}

bool Handle_RequestFriend_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	FriendPtr friends = remotePlayer->GetFriend();
	if (nullptr == friends)
	{
		return false;
	}

	int32 error		= inCommand.GetReturnParam();
	int32 action	= inCommand.GetParam(L"@action");
	if (error == ErrorToInt(EDCommonErrorType::SUCCESS))
	{
		Handle_LoadFriendList_Request(inSession, remotePlayer->GetToken().GetCharacterID(), 1);
	}

	Protocol::S2C_RequestFriend requestFriendPacket;
	requestFriendPacket.set_error(error);
	requestFriendPacket.set_action(action);
	requestFriendPacket.set_timestamp(remotePlayer->GetWorld().lock()->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, requestFriendPacket);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_BlockFriend_Request(PacketSessionPtr& inSession, const int64& inUserCharacterID, const std::string& inFriendName, const int32& inAction)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id		= inUserCharacterID;
	ADOVariant friend_name		= inFriendName.c_str();
	ADOVariant action			= inAction;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.block_friend_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@friend_name", friend_name);
	command.SetInputParam(L"@action", action);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_BlockFriend_Response);
	return true;
}

bool Handle_BlockFriend_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	FriendPtr friends = remotePlayer->GetFriend();
	if (nullptr == friends)
	{
		return false;
	}

	int32 error		= inCommand.GetReturnParam();
	int32 action	= inCommand.GetParam(L"@action");
	if (error == ErrorToInt(EDCommonErrorType::SUCCESS))
	{
		Handle_LoadFriendList_Request(inSession, remotePlayer->GetToken().GetCharacterID(), 2);
	}

	Protocol::S2C_BlockFriend blockFriendPacket;
	blockFriendPacket.set_error(error);
	blockFriendPacket.set_action(action);
	blockFriendPacket.set_timestamp(remotePlayer->GetWorld().lock()->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, blockFriendPacket);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_FindFriend_Request(PacketSessionPtr& inSession, const std::string& inFriendName)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	//TODO:
	ADOVariant friend_name = inFriendName.c_str();
	ADOVariant friend_id;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.find_friend_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@friend_name", friend_name);
	command.SetOutputParam(L"@friend_id", DataTypeEnum::adInteger, friend_id);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_FindFriend_Response);
	return true;
}

bool Handle_FindFriend_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	return false;
}
