#pragma once

class GameDataBaseHandler
{
public:
	static bool PushAsyncTask(PacketSessionPtr& inSession, ADOConnection& inADOConnection, ADOCommand& inADOCommand, ADORecordset& inADORecordset, ADOCallBack inADOCallBack)
	{

		SessionManagerPtr sessionManager = inSession->GetSessionManager();
		if (nullptr == sessionManager)
		{
			return false;
		}

		ServicePtr service = sessionManager->GetService();
		if (nullptr == service)
		{
			return false;
		}

		DatabaseManagerPtr	databaseManager = service->GetDatabaseManager();
		if (nullptr == databaseManager)
		{
			return false;
		}

		const bool result = databaseManager->PushAsyncTaskQueue(inSession, inADOConnection, inADOCommand, inADORecordset, inADOCallBack);
		return result;
	}
};

