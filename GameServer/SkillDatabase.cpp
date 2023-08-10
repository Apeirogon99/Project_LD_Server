#include "pch.h"
#include "SkillDatabase.h"

bool Handle_LoadSkillTree_Request(PacketSessionPtr& inSession, const int64& inCharacterID)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inCharacterID;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_skill_tree_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_LoadSkillTree_Response);
	return true;
}

bool Handle_LoadSkillTree_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	SkillPtr skill = remotePlayer->GetSkill();
	if (nullptr == skill)
	{
		return false;
	}
	int64 playerCharacterID = inCommand.GetParam(L"@character_id");

	int32 error = inCommand.GetReturnParam();
	if (error == ErrorToInt(EDCommonErrorType::SUCCESS))
	{
		Protocol::S2C_LoadSkillTree loadSkillTree;

		while (!inRecordset.IsEof())
		{
			int32 skillID		= inRecordset.GetFieldItem(L"skill_id");
			int32 skillCount	= inRecordset.GetFieldItem(L"count");

			loadSkillTree.add_skill_id(skillID);
			loadSkillTree.add_skill_count(skillCount);

			skill->LoadSkillTree(skillID, skillCount);
			inRecordset.MoveNext();
		}

		loadSkillTree.set_timestamp(remotePlayer->GetWorld().lock()->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, loadSkillTree);
		inSession->Send(sendBuffer);
	}

	skill->SetLoadSkill(error == ErrorToInt(EDCommonErrorType::SUCCESS));
	remotePlayer->OnLoadComplete();
	return true;
}

bool Handle_UpdateSkillTree_Request(PacketSessionPtr& inSession, const int64& inCharacterID, const int32& inSkillID, const int32& inSkillCount)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant character_id = inCharacterID;
	ADOVariant skill_id		= inSkillID;
	ADOVariant skill_count	= inSkillCount;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.update_skill_tree_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);
	command.SetInputParam(L"@skill_id", skill_id);
	command.SetInputParam(L"@skill_count", skill_count);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_UpdateSkillTree_Response);
	return true;
}

bool Handle_UpdateSkillTree_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	SkillPtr skill = remotePlayer->GetSkill();
	if (nullptr == skill)
	{
		return false;
	}

	int32 error			= inCommand.GetReturnParam();
	int32 skillID		= inCommand.GetParam(L"@skill_id");
	int32 skillCount	= inCommand.GetParam(L"@skill_count");
	if (error != ErrorToInt(EDCommonErrorType::SUCCESS))
	{
		skill->RollbackSkillTree(skillID, skillCount);
	}

	Protocol::S2C_UpdateSkillTree updateSkillTree;
	updateSkillTree.set_error(error);
	updateSkillTree.set_skill_id(skillID);
	updateSkillTree.set_skill_count(skillCount);
	updateSkillTree.set_timestamp(remotePlayer->GetWorld().lock()->GetWorldTime());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(inSession, updateSkillTree);
	inSession->Send(sendBuffer);
	return true;
}
