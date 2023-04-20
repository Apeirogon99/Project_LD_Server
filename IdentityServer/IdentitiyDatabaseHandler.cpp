#include "pch.h"
#include "IdentitiyDatabaseHandler.h"

using namespace std;

bool Handle_Singin_Requset(PacketSessionPtr& inSession, Protocol::C2S_Singin& inPacket)
{
	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"account_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
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

	IdentitiyDatabaseHandler::AddADOTask(inSession, singinCommand, singinRecordSet, Handle_Singin_Response);

	return true;
}

bool Handle_Singin_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->mRemotePlayer;
	if (remotePlayer == nullptr)
	{
		return false;
	}

	Protocol::S2C_Singin singinPacket;
	int32 ret = inCommand.GetReturnParam();
	if (ret != 0)
	{
		singinPacket.set_error(ret);
		singinPacket.set_token("");
	}
	else
	{

		int32 globalID = inCommand.GetOutputParam(L"@global_id");
		std::string token = inCommand.GetOutputParam(L"@token");

		remotePlayer->mToken = token;
		int64 remoteID = remotePlayer->mRemoteID;

		remotePlayer->mGlobalID = globalID;
		singinPacket.set_error(ret);
		singinPacket.set_token(token);
		singinPacket.set_remote_id(remoteID);
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, singinPacket);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_Singup_Requset(PacketSessionPtr& inSession, Protocol::C2S_Singup& inPacket)
{
	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"account_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
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

	IdentitiyDatabaseHandler::AddADOTask(inSession, command, recordset, Handle_Singup_Response);

	return true;
}

bool Handle_Singup_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->mRemotePlayer;
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
		int64 remoteID = remotePlayer->mRemoteID;

		remotePlayer->mGlobalID = globalID;
		singupPacket.set_error(ret);
		singupPacket.set_remote_id(remoteID);
	}

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

	RemotePlayerPtr remotePlayer = playerState->mRemotePlayer;
	if (remotePlayer == nullptr)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"account_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant global_id = remotePlayer->mGlobalID;
	if (static_cast<int32>(global_id) > -1)
	{
		return false;
	}
	ADOVariant verifyCode = atoi(inPacket.verified_code().c_str());

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.check_email_verify_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@global_id", global_id);
	command.SetInputParam(L"@code", verifyCode);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	IdentitiyDatabaseHandler::AddADOTask(inSession, command, recordset, Handle_EmailVerified_Response);

	return true;
}

bool Handle_EmailVerified_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->mRemotePlayer;
	if (remotePlayer == nullptr)
	{
		return false;
	}

	Protocol::S2C_EmailVerified emailVerifiedPacket;
	int32 ret = inCommand.GetReturnParam();

	int64 remoteID = remotePlayer->mRemoteID;
	emailVerifiedPacket.set_error(ret);
	emailVerifiedPacket.set_remote_id(remoteID);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, emailVerifiedPacket);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_LoadCharacters_Requset(PacketSessionPtr& inSession, Protocol::C2S_LoadCharacters& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->mRemotePlayer;
	if (remotePlayer == nullptr)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant global_id = remotePlayer->mGlobalID;
	ADOVariant server_id = inPacket.server_id();

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_character_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@global_id", global_id);
	command.SetInputParam(L"@server_id", server_id);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	IdentitiyDatabaseHandler::AddADOTask(inSession, command, recordset, Handle_LoadCharacters_Response);

	return true;
}

bool Handle_LoadCharacters_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	Protocol::S2C_LoadCharacters loadCharacterPackets;
	int32 loadCharacterPacketIndex = 0;
	if (inRecordset.IsOpen())
	{
		while (!(inRecordset.IsEof()))
		{
			std::string name			= inRecordset.GetFieldItem(L"name");
			int32		characterClass	= inRecordset.GetFieldItem(L"character_calss_id");
			int32		race			= inRecordset.GetFieldItem(L"race_id");
			//int32		level			= inRecordset.GetFieldItem(L"level");
			int32		seat			= inRecordset.GetFieldItem(L"seat");
			int32		skin_color		= inRecordset.GetFieldItem(L"skin_color");
			int32		hair_color		= inRecordset.GetFieldItem(L"hair_color");
			int32		eye_color		= inRecordset.GetFieldItem(L"eye_color");
			int32		eyebrow_color	= inRecordset.GetFieldItem(L"eyebrow_color");


			int32		hair			= inRecordset.GetFieldItem(L"hair");
			int32		helmet			= inRecordset.GetFieldItem(L"helmet");
			int32		shoulders		= inRecordset.GetFieldItem(L"shoulders");
			int32		chest			= inRecordset.GetFieldItem(L"chest_add");
			int32		bracers			= inRecordset.GetFieldItem(L"bracers_add");
			int32		hands			= inRecordset.GetFieldItem(L"hands_add");
			int32		pants			= inRecordset.GetFieldItem(L"pants_add");
			int32		boots			= inRecordset.GetFieldItem(L"boots");
			int32		weapon_l		= inRecordset.GetFieldItem(L"weapon_l");
			int32		weapon_r		= inRecordset.GetFieldItem(L"weapon_r");

			Protocol::SCharacterData* characterData = loadCharacterPackets.add_character_data();
			characterData->set_name(name);
			characterData->set_level(1);
			characterData->set_character_class(static_cast<Protocol::ECharacterClass>(characterClass));

			Protocol::SCharacterAppearance* appearances = characterData->mutable_appearance();
			appearances->set_race(static_cast<Protocol::ERace>(race));
			appearances->set_seat(seat);
			appearances->set_skin_color(skin_color);
			appearances->set_hair_color(hair_color);
			appearances->set_eye_color(eye_color);
			appearances->set_eyebrow_color(eyebrow_color);

			Protocol::SCharacterEqipment* eqipments = characterData->mutable_eqipment();
			eqipments->set_hair(hair);
			eqipments->set_helmet(helmet);
			eqipments->set_shoulders(shoulders);
			eqipments->set_chest(chest);
			eqipments->set_bracers(bracers);
			eqipments->set_hands(hands);
			eqipments->set_pants(pants);
			eqipments->set_boots(boots);
			eqipments->set_weapon_l(weapon_l);
			eqipments->set_weapon_r(weapon_r);

			loadCharacterPacketIndex++;
			inRecordset.MoveNext();
		}
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, loadCharacterPackets);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_CreateCharacter_Requset(PacketSessionPtr& inSession, Protocol::C2S_CreateCharacter& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->mRemotePlayer;
	if (remotePlayer == nullptr)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	const Protocol::SCharacterData& characterData = inPacket.character_data();
	const Protocol::SCharacterAppearance& characterAppearance = characterData.appearance();

	ADOVariant name				= characterData.name().c_str();
	ADOVariant characterClass	= characterData.character_class();
	ADOVariant race				= characterAppearance.race();
	ADOVariant seat				= characterAppearance.seat();
	ADOVariant global_id		= remotePlayer->mGlobalID;
	ADOVariant server_id		= inPacket.server_id();

	ADOVariant skin_color		= characterAppearance.skin_color();
	ADOVariant hair_color		= characterAppearance.hair_color();
	ADOVariant eye_color		= characterAppearance.eye_color();
	ADOVariant eyebrow_color	= characterAppearance.eyebrow_color();

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.create_character_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@name", name);
	command.SetInputParam(L"@class", characterClass);
	command.SetInputParam(L"@race", race);
	command.SetInputParam(L"@seat", seat);
	command.SetInputParam(L"@global_id", global_id);
	command.SetInputParam(L"@server_id", server_id);

	command.SetInputParam(L"@skin_color", skin_color);
	command.SetInputParam(L"@hair_color", hair_color);
	command.SetInputParam(L"@eye_color", eye_color);
	command.SetInputParam(L"@eyebrow_color", eyebrow_color);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	IdentitiyDatabaseHandler::AddADOTask(inSession, command, recordset, Handle_CreateCharacter_Response);

	return true;
}

bool Handle_CreateCharacter_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	Protocol::S2C_CreateCharacter createCharacterPacket;
	int32 ret = inCommand.GetReturnParam();
	if (ret != 0)
	{
		createCharacterPacket.set_error(ret);
	}
	else
	{
		createCharacterPacket.set_error(ret);
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, createCharacterPacket);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_UpdateAppearance_Requset(PacketSessionPtr& inSession, Protocol::C2S_UpdateAppearance& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant characterID		= 1;
	ADOVariant skin_color		= inPacket.appearance().skin_color();
	ADOVariant hair_color		= inPacket.appearance().hair_color();
	ADOVariant eye_color		= inPacket.appearance().eye_color();
	ADOVariant eyebrow_color	= inPacket.appearance().eyebrow_color();

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.update_appearance_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", characterID);
	command.SetInputParam(L"@skin_color", skin_color);
	command.SetInputParam(L"@hair_color", hair_color);
	command.SetInputParam(L"@eye_color", eye_color);
	command.SetInputParam(L"@eyebrow_color", eyebrow_color);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	IdentitiyDatabaseHandler::AddADOTask(inSession, command, recordset, Handle_UpdateAppearance_Response);

	return true;
}

bool Handle_UpdateAppearance_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}


	return true;
}

bool Handle_DeleteCharacter_Requset(PacketSessionPtr& inSession, Protocol::C2S_DeleteCharacter& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOCommand command;

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	IdentitiyDatabaseHandler::AddADOTask(inSession, command, recordset, Handle_DeleteCharacter_Response);

	return true;
}

bool Handle_DeleteCharacter_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	return true;
}

bool Handle_UpdateNickName_Requset(PacketSessionPtr& inSession, Protocol::C2S_UpdateNickName& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOCommand command;

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	IdentitiyDatabaseHandler::AddADOTask(inSession, command, recordset, Handle_UpdateNickName_Response);

	return true;
}

bool Handle_UpdateNickName_Response(PacketSessionPtr& inSession, ADOCommand& inCommand, ADORecordset& inRecordset)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	return true;
}
