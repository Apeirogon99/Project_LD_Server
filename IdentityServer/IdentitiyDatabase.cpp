#include "pch.h"
#include "IdentitiyDatabase.h"

bool Handle_Singin_Requset(PacketSessionPtr& inSession, Protocol::C2S_Singin& inPacket)
{
	ADOConnectionInfo ConnectionInfo(CommonAccountDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant userName = inPacket.user_id().c_str();
	ADOVariant userPassword = inPacket.user_password().c_str();

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
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (remotePlayer == nullptr)
	{
		return false;
	}

	Protocol::S2C_Singin singinPacket;
	int32 ret = inCommand.GetReturnParam();
	if (ret != GetDatabaseErrorToInt(EDCommonErrorType::SUCCESS))
	{
		singinPacket.set_error(ret);
		singinPacket.set_token("");
	}
	else
	{
		int32		globalID	= inCommand.GetOutputParam(L"@global_id");
		std::string token		= inCommand.GetOutputParam(L"@token");

		remotePlayer->SetServerID(1);
		remotePlayer->SetGlobalID(globalID);
		remotePlayer->SetToken(token);
		remotePlayer->SetRoomType(ERoomType::SelectRoom);
	}

	singinPacket.set_error(ret);
	singinPacket.set_token(remotePlayer->GetToken());
	singinPacket.set_remote_id(remotePlayer->GetGameObjectID());

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, singinPacket);
	inSession->Send(sendBuffer);
	return true;
}

bool Handle_Singup_Requset(PacketSessionPtr& inSession, Protocol::C2S_Singup& inPacket)
{
	ADOConnectionInfo ConnectionInfo(CommonAccountDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	std::string tempEmail = inPacket.user_email();
	size_t find = tempEmail.find('@');

	ADOVariant userID = inPacket.user_id().c_str();
	ADOVariant userPS = inPacket.user_password().c_str();
	ADOVariant local = tempEmail.substr(0, find).c_str();
	ADOVariant domain = tempEmail.substr(find + 1, tempEmail.length()).c_str();

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
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (remotePlayer == nullptr)
	{
		return false;
	}

	Protocol::S2C_Singup singupPacket;
	int32 ret = inCommand.GetReturnParam();
	if (ret != 0)
	{
		singupPacket.set_error(ret);
	}
	else
	{
		int32 globalID = inCommand.GetOutputParam(L"@global_id");
	}

	singupPacket.set_error(ret);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, singupPacket);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_EmailVerified_Requset(PacketSessionPtr& inSession, Protocol::C2S_EmailVerified& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (remotePlayer == nullptr)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(CommonAccountDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant global_id = remotePlayer->GetGlobalID();
	ADOVariant verifyCode = atoi(inPacket.verified_code().c_str());

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

bool Handle_EmailVerified_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (remotePlayer == nullptr)
	{
		return false;
	}

	int32 ret		= inCommand.GetReturnParam();
	int64 remoteID	= remotePlayer->GetGameObjectID();

	Protocol::S2C_EmailVerified emailVerifiedPacket;
	emailVerifiedPacket.set_error(ret);
	emailVerifiedPacket.set_remote_id(remoteID);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, emailVerifiedPacket);
	inSession->Send(sendBuffer);

	return true;
}