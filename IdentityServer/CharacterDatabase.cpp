#include "pch.h"
#include "CharacterDatabase.h"

using namespace std;

bool Handle_LoadCharacters_Requset(PacketSessionPtr& inSession, const int32& inGlobalID, const int32& inServerID)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant global_id = inGlobalID;
	ADOVariant server_id = inServerID;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_character_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@global_id", global_id);
	command.SetInputParam(L"@server_id", server_id);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	DatabaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_LoadCharacters_Response);
	return true;
}

bool Handle_LoadCharacters_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	Protocol::S2C_LoadCharacters loadCharacterPackets;
	int32 ret = inCommand.GetReturnParam();
	if (ret == 0)
	{
		if (!inRecordset.IsOpen())
		{
			inRecordset.Open(inCommand, inConnection);
		}

		while (!inRecordset.IsEof())
		{
			int32		characterID		= inRecordset.GetFieldItem(L"id");
			std::string name			= inRecordset.GetFieldItem(L"name");
			int32		race			= inRecordset.GetFieldItem(L"race_id");
			int32		characterClass	= inRecordset.GetFieldItem(L"character_calss_id");
			int32		seat			= inRecordset.GetFieldItem(L"seat");
			int32		skin_color		= inRecordset.GetFieldItem(L"skin_color");
			int32		hair_color		= inRecordset.GetFieldItem(L"hair_color");
			int32		eye_color		= inRecordset.GetFieldItem(L"eye_color");
			int32		eyebrow_color	= inRecordset.GetFieldItem(L"eyebrow_color");


			int32		hair			= inRecordset.GetFieldItem(L"hair");
			int32		helmet			= inRecordset.GetFieldItem(L"helmet");
			int32		shoulders		= inRecordset.GetFieldItem(L"shoulders");
			int32		chest			= inRecordset.GetFieldItem(L"chest");
			int32		bracers			= inRecordset.GetFieldItem(L"bracers");
			int32		hands			= inRecordset.GetFieldItem(L"hands");
			int32		pants			= inRecordset.GetFieldItem(L"pants");
			int32		boots			= inRecordset.GetFieldItem(L"boots");
			int32		weapon_l		= inRecordset.GetFieldItem(L"weapon_l");
			int32		weapon_r		= inRecordset.GetFieldItem(L"weapon_r");

			int32		level			= inRecordset.GetFieldItem(L"level");

			Protocol::SCharacterData* characterData = loadCharacterPackets.add_character_data();
			characterData->set_name(name.c_str());
			characterData->set_level(level);
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

			remotePlayer->GetCharacterManager()->PushCharacter(characterID, characterData);

			inRecordset.MoveNext();
		}
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, loadCharacterPackets);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_CreateCharacter_Requset(PacketSessionPtr& inSession, const std::string& inName, const int32& inClass, const int32& inRace, const int32& inSeat, const int32& inGlobalID, const int32& inServerID, const int32& inSkinColor, const int32& inHairID, const int32& inHairColor, const int32& inEyeColor, const int32& inEyebrowColor)
{

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant name				= inName.c_str();
	ADOVariant characterClass	= inClass;
	ADOVariant race				= inRace;
	ADOVariant seat				= inSeat;
	ADOVariant global_id		= inGlobalID;
	ADOVariant server_id		= inServerID;

	ADOVariant skin_color		= inSkinColor;
	ADOVariant hair_color		= inHairColor;
	ADOVariant eye_color		= inEyeColor;
	ADOVariant eyebrow_color	= inEyebrowColor;

	ADOVariant hair				= inHairID;

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

	command.SetInputParam(L"@hair", hair);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	DatabaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_CreateCharacter_Response);

	return true;
}

bool Handle_CreateCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	int32 ret = inCommand.GetReturnParam();
	if (ret == GetDatabaseErrorToInt(EDCommonErrorType::FAILURE))
	{
		//TODO:
	}

	Protocol::S2C_CreateCharacter createCharacterPacket;
	createCharacterPacket.set_error(ret);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, createCharacterPacket);
	inSession->Send(sendBuffer);

	return true;
}

bool Handle_DeleteCharacter_Requset(PacketSessionPtr& inSession, const int32& inGlobalID, const int32& inServerID, const int32& inCharacterID)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant global_id	= inGlobalID;
	ADOVariant server_id	= inServerID;
	ADOVariant character_id = inCharacterID;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.delete_character_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@global_id", global_id);
	command.SetInputParam(L"@server_id", server_id);
	command.SetInputParam(L"@character_id", character_id);

	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	DatabaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_DeleteCharacter_Response);
	return true;
}

bool Handle_DeleteCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	int32 ret = inCommand.GetReturnParam();
	if (ret == GetDatabaseErrorToInt(EDCommonErrorType::FAILURE))
	{
		//TODO:
	}

	Protocol::S2C_DeleteCharacter deleteCharacterPacket;
	deleteCharacterPacket.set_error(ret);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, deleteCharacterPacket);
	inSession->Send(sendBuffer);
	return true;
}