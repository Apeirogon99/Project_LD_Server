#include "pch.h"
#include "CharacterDatabase.h"

using namespace std;

bool Handle_LoadCharacters_Requset(PacketSessionPtr& inSession, Protocol::C2S_LoadCharacters& inPacket)
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

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	ADOVariant global_id = remotePlayer->GetGlobalID();
	ADOVariant server_id = remotePlayer->GetServerID();

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
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	Protocol::S2C_LoadCharacters loadCharacterPackets;
	std::vector<CharacterPtr> newCharacters(MAX_CHARACTER);
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

			CharacterPtr newCharacter = std::make_shared<Character>();
			newCharacter->LoadCharacter(characterID, *characterData);
			newCharacters[seat] = newCharacter;

			inRecordset.MoveNext();
		}
	}

	remotePlayer->SetCharacters(newCharacters);

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

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (remotePlayer == nullptr)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	const Protocol::SCharacterData& characterData = inPacket.character_data();
	const Protocol::SCharacterAppearance& characterAppearance = characterData.appearance();
	const Protocol::SCharacterEqipment& characterEqipment = characterData.eqipment();

	ADOVariant name = characterData.name().c_str();
	ADOVariant characterClass = characterData.character_class();
	ADOVariant race = characterAppearance.race();
	ADOVariant seat = characterAppearance.seat();
	ADOVariant global_id = remotePlayer->GetGlobalID();
	ADOVariant server_id = remotePlayer->GetServerID();

	ADOVariant skin_color = characterAppearance.skin_color();
	ADOVariant hair_color = characterAppearance.hair_color();
	ADOVariant eye_color = characterAppearance.eye_color();
	ADOVariant eyebrow_color = characterAppearance.eyebrow_color();

	ADOVariant hair = characterEqipment.hair();

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
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	Protocol::S2C_CreateCharacter createCharacterPacket;
	int32 ret = inCommand.GetReturnParam();
	if (ret != GetDatabaseErrorToInt(EDCommonErrorType::SUCCESS))
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

bool Handle_DeleteCharacter_Requset(PacketSessionPtr& inSession, Protocol::C2S_DeleteCharacter& inPacket)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(inSession);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	RemotePlayerPtr remotePlayer = playerState->GetRemotePlayer();
	if (nullptr == remotePlayer)
	{
		return false;
	}

	ADOVariant global_id = remotePlayer->GetGlobalID();
	ADOVariant server_id = remotePlayer->GetServerID();
	
	CharacterPtr deleteCharacter = remotePlayer->GetCharacter(inPacket.name().c_str());
	if (nullptr == deleteCharacter)
	{
		return false;
	}
	ADOVariant character_id = deleteCharacter->GetID();

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
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	Protocol::S2C_DeleteCharacter deleteCharacterPacket;
	int32 ret = inCommand.GetReturnParam();
	deleteCharacterPacket.set_error(ret);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(inSession, deleteCharacterPacket);
	inSession->Send(sendBuffer);

	return true;
}