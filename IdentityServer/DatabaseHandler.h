#pragma once

class DatabaseHandler
{
public:
	static bool PushAsyncTask(PacketSessionPtr& inSession, ADOConnection& inADOConnection, ADOCommand& inADOCommand, ADORecordset& inADORecordset, ADOCallBack inADOCallBack)
	{
		SessionManagerPtr sessionManager = inSession->GetSessionManager();
		if (sessionManager == nullptr)
		{
			return false;
		}

		ServicePtr service = sessionManager->GetService();
		if (service == nullptr)
		{
			return false;
		}

		DatabaseManagerPtr databaseManager = service->GetDatabaseManager();
		if (databaseManager == nullptr)
		{
			return false;
		}

		bool result = databaseManager->PushAsyncTaskQueue(inSession, inADOConnection, inADOCommand, inADORecordset, inADOCallBack);
		return result;
	}
};

