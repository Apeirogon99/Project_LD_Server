#include "pch.h"
#include "IdentitiyDatabaseHandler.h"

using namespace std;

void Handle_Singin_Requset(PacketSessionPtr& session, Protocol::C2S_Singin& pkt)
{
	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"account_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection accountConnection;
	accountConnection.Open(ConnectionInfo);

	ADOVariant userName = pkt.user_id().c_str();
	ADOVariant userPassword = pkt.user_password().c_str();

	ADOCommand singinCommand;
	singinCommand.SetStoredProcedure(accountConnection, L"dbo.singin_sp");
	singinCommand.SetReturnParam();
	singinCommand.SetInputParam(L"@name", userName);
	singinCommand.SetInputParam(L"@password", userPassword);
	singinCommand.SetOutputParam(L"@global_id", DataTypeEnum::adInteger, sizeof(int32));
	singinCommand.SetOutputParam(L"@token", DataTypeEnum::adBSTR, 36);

	ADORecordset singinRecordSet;
	singinCommand.ExecuteStoredProcedure(singinRecordSet, EExcuteReturnType::Async_Return);

	IdentitiyDatabaseHandler::AddADOWork(session, singinCommand, singinRecordSet, Handle_Singin_Response);
}

void Handle_Singin_Response(PacketSessionPtr& session, ADOCommand& singinCommand, ADORecordset& singinRecordSet)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return;
	}

	Protocol::S2C_Singin singinPacket;
	int32 ret = singinCommand.GetReturnParam();
	if (ret != 0)
	{
		singinPacket.set_error(ret);
		singinPacket.set_token("");
	}
	else
	{

		int32 globalID = singinCommand.GetOutputParam(L"@global_id");
		std::string token = singinCommand.GetOutputParam(L"@token");

		playerState->mToken = token;
		int64 remoteID = playerState->mRemoteID;

		playerState->mGlobalID = globalID;
		singinPacket.set_error(ret);
		singinPacket.set_token(token);
		singinPacket.set_remote_id(remoteID);
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, singinPacket);
	session->Send(sendBuffer);
}
