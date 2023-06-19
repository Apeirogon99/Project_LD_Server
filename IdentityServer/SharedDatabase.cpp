#include "pch.h"
#include "SharedDatabase.h"

bool Handle_LoadServerList_Requset(const EServerType& inServerType, std::vector<Protocol::SServerInfo>& outServerInfo)
{

	ADOConnectionInfo ConnectionInfo(CommonSharedDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant server_type = static_cast<int32>(inServerType);

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_server_list");
	command.SetReturnParam();
	command.SetInputParam(L"@server_type", server_type);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Wait_Return);

	int32 ret = command.GetReturnParam();
	if (ret != 0)
	{
		return false;
	}

	if (!recordset.IsOpen())
	{
		recordset.Open(command, connection);
	}

	while (!recordset.IsEof())
	{
		int32		server_id	= recordset.GetFieldItem(L"id");
		std::string server_ip	= recordset.GetFieldItem(L"ip");
		int32		server_port = recordset.GetFieldItem(L"port");
		std::string server_name = recordset.GetFieldItem(L"name");

		Protocol::SServerInfo tempInfo;
		tempInfo.set_id(server_id);
		tempInfo.set_ip(server_ip);
		tempInfo.set_port(server_port);
		tempInfo.set_name(server_name);
		outServerInfo.push_back(tempInfo);

		recordset.MoveNext();
	}

	return true;
}