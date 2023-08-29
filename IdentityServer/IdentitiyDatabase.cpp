#include "pch.h"
#include "IdentitiyDatabase.h"

bool Handle_Singin_Requset(PacketSessionPtr& inSession, const std::string& inUserName, const std::string& inUserPassword)
{
	ADOConnectionInfo ConnectionInfo(CommonAccountDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant userName = inUserName.c_str();
	ADOVariant userPassword = inUserPassword.c_str();

	ADOCommand singinCommand;
	singinCommand.SetStoredProcedure(connection, L"dbo.singin_sp");
	singinCommand.SetReturnParam();
	singinCommand.SetInputParam(L"@name", userName);
	singinCommand.SetInputParam(L"@password", userPassword);
	singinCommand.SetOutputParam(L"@global_id", DataTypeEnum::adInteger, sizeof(int32));
	singinCommand.SetOutputParam(L"@token", DataTypeEnum::adBSTR, 36);

	ADORecordset singinRecordSet;
	singinCommand.ExecuteStoredProcedure(singinRecordSet, EExcuteReturnType::Async_Return);

	DatabaseHandler::PushAsyncTask(inSession, connection, singinCommand, singinRecordSet, Handle_Singin_Response);

	return true;
}

bool Handle_Singin_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return false;
	}

	Protocol::S2C_Singin singinPacket;
	int32 ret = inCommand.GetReturnParam();
	if (ret == GetDatabaseErrorToInt(EDCommonErrorType::SUCCESS))
	{
		int32		globalID	= inCommand.GetOutputParam(L"@global_id");
		std::string token		= inCommand.GetOutputParam(L"@token");

		identityManager->SetGlobalID(globalID);
		identityManager->SetToken(token);
	}

	singinPacket.set_error(ret);
	singinPacket.set_token(identityManager->GetToken());

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, singinPacket);
	inSession->Send(sendBuffer);
	return true;
}

bool Handle_Singup_Requset(PacketSessionPtr& inSession, const std::string& inUserName, const std::string& inUserPassword, const std::string& inLocal, const std::string& inDomain)
{
	ADOConnectionInfo ConnectionInfo(CommonAccountDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant userID	= inUserName.c_str();
	ADOVariant userPS	= inUserPassword.c_str();
	ADOVariant local	= inLocal.c_str();
	ADOVariant domain	= inDomain.c_str();

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.singup_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@name", userID);
	command.SetInputParam(L"@password", userPS);
	command.SetInputParam(L"@local", local);
	command.SetInputParam(L"@domain", domain);
	command.SetOutputParam(L"@global_id", DataTypeEnum::adInteger, sizeof(int32));

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	DatabaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_Singup_Response);

	return true;
}

bool Handle_Singup_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return false;
	}

	Protocol::S2C_Singup singupPacket;
	int32 ret = inCommand.GetReturnParam();
	if (ret == GetDatabaseErrorToInt(EDCommonErrorType::SUCCESS) && ret == GetDatabaseErrorToInt(EDCommonErrorType::TEMP_VERIFY))
	{
		int32 globalID = inCommand.GetOutputParam(L"@global_id");
		identityManager->SetGlobalID(globalID);
	}
	singupPacket.set_error(ret);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, singupPacket);
	inSession->Send(sendBuffer);
	return true;
}

bool Handle_EmailVerified_Requset(PacketSessionPtr& inSession, const int32& inGlobalID, const int32& inVeriftyCode)
{
	ADOConnectionInfo ConnectionInfo(CommonAccountDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant global_id	= inGlobalID;
	ADOVariant verifyCode	= inVeriftyCode;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.check_email_verify_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@global_id", global_id);
	command.SetInputParam(L"@code", verifyCode);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	DatabaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_EmailVerified_Response);
	return true;
}

bool Handle_Select_Character_Request(PacketSessionPtr& inSession, const int32 inGlobalID)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant globalID = inGlobalID;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.select_character_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@global_id", globalID);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	DatabaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_Select_Character_Response);
	return true;
}

bool Handle_EmailVerified_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return false;
	}

	Protocol::S2C_EmailVerified emailVerifiedPacket;
	int32 ret = inCommand.GetReturnParam();
	if (ret == GetDatabaseErrorToInt(EDCommonErrorType::SUCCESS))
	{
		int64 remoteID = remotePlayer->GetGameObjectID();
	}
	emailVerifiedPacket.set_error(ret);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, emailVerifiedPacket);
	inSession->Send(sendBuffer);
	return true;
}

int32 GetLoadServer(std::vector<std::pair<int32, int32>>& inCharacterInfo, const int32 inServerID)
{
	for (int32 index = 0; index < inCharacterInfo.size(); index++)
	{
		std::pair<int32, int32> info = inCharacterInfo[index];
		if (info.first == inServerID)
		{
			return info.second;
		}
	}

	return 0;
}

bool Handle_Select_Character_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	if (nullptr == playerState)
	{
		return false;
	}

	GameStatePtr gameState = playerState->GetGameState();
	if (nullptr == gameState)
	{
		return false;
	}

	IdentityTaskPtr task = gameState->GetTask();
	if (nullptr == task)
	{
		return false;
	}

	LoginWorldPtr world = task->GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	LoginRemotePlayerPtr remotePlayer = std::static_pointer_cast<LoginRemotePlayer>(playerState->GetRemotePlayer());
	if (nullptr == remotePlayer)
	{
		return false;
	}

	IdentityManagerPtr identityManager = remotePlayer->GetIdentityManager();
	if (nullptr == identityManager)
	{
		return false;
	}

	
	int32 ret = inCommand.GetReturnParam();
	if (ret != GetDatabaseErrorToInt(EDCommonErrorType::SUCCESS))
	{
		return false;
	}

	const std::vector<Protocol::SServerInfo>& serverInfos = world->GetServerInfo();
	std::vector<std::pair<int32, int32>> loadCharacter;

	bool isLoad = false;
	Protocol::S2C_LoadServer loadServerPacket;
	while (!inRecordset.IsEof())
	{
		int32 server_id			= inRecordset.GetFieldItem(L"server_id");
		int32 character_count	= inRecordset.GetFieldItem(L"character_count");

		loadCharacter.push_back(std::make_pair(server_id, character_count));
		inRecordset.MoveNext();
	}

	for (int32 index = 0; index < serverInfos.size(); index++)
	{
		const Protocol::SServerInfo& info = serverInfos[index];

		int32 characterCount = GetLoadServer(loadCharacter, info.id());
		loadServerPacket.add_id(info.id());
		loadServerPacket.add_name(info.name());
		loadServerPacket.add_state(1);
		loadServerPacket.add_count(characterCount);
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, loadServerPacket);
	inSession->Send(sendBuffer);
	return true;
}
