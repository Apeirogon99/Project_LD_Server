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

		DatabaseTaskQueuePtr handler = databaseManager->GetDatabaseTaskQueue();
		if (handler == nullptr)
		{
			return false;
		}

		const bool result = handler->PushAsyncTaskQueue(inSession, inADOConnection, inADOCommand, inADORecordset, inADOCallBack);
		return result;
	}
};
