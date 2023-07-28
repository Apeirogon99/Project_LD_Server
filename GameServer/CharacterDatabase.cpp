#include "pch.h"
#include "CharacterDatabase.h"

bool Handle_LoadCharacter_Request(PacketSessionPtr& inSession, const int32 characterID)
{
	ADOConnectionInfo ConnectionInfo(CommonGameDatabaseInfo);
	ADOConnection connection;
	connection.Open(ConnectionInfo);

	//TODO:
	ADOVariant character_id = characterID;

	ADOCommand command;
	command.SetStoredProcedure(connection, L"dbo.load_game_character_sp");
	command.SetReturnParam();
	command.SetInputParam(L"@character_id", character_id);


	ADORecordset recordset;
	command.ExecuteStoredProcedure(recordset, EExcuteReturnType::Async_Return);

	GameDataBaseHandler::PushAsyncTask(inSession, connection, command, recordset, Handle_LoadCharacter_Response);
	return true;
}

bool Handle_LoadCharacter_Response(PacketSessionPtr& inSession, ADOConnection& inConnection, ADOCommand& inCommand, ADORecordset& inRecordset)
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

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}

	int32 ret = inCommand.GetReturnParam();
	if (ret != 0)
	{
		return false;
	}

	int32		id				= inCommand.GetParam(L"@character_id");

	std::string name			= inRecordset.GetFieldItem(L"name");

	int32		race			= inRecordset.GetFieldItem(L"race_id");
	int32		characterClass	= inRecordset.GetFieldItem(L"character_calss_id");
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
	int32		experience		= inRecordset.GetFieldItem(L"experience");

	int32		amount			= inRecordset.GetFieldItem(L"amount");

	Protocol::SCharacterData loadCharacterData;
	loadCharacterData.set_name(name.c_str());
	loadCharacterData.set_level(level);
	loadCharacterData.set_experience(experience);
	loadCharacterData.set_character_class(static_cast<Protocol::ECharacterClass>(characterClass));

	Protocol::SCharacterAppearance* appearances = loadCharacterData.mutable_appearance();
	appearances->set_race(static_cast<Protocol::ERace>(race));
	appearances->set_skin_color(skin_color);
	appearances->set_hair_color(hair_color);
	appearances->set_eye_color(eye_color);
	appearances->set_eyebrow_color(eyebrow_color);

	Protocol::SCharacterEqipment* eqipments = loadCharacterData.mutable_eqipment();
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

	PlayerCharacterPtr character = remotePlayer->GetCharacter();
	if (nullptr == character)
	{
		return false;
	}

	character->SetCharacterID(id);
	character->SetCharacterData(loadCharacterData);

	character->SetLocation(800.0f, 100.0f, 480.0f);
	character->GetMovementComponent().InitMovement(character->GetLocation(), GAME_TICK, world->GetWorldTime());
	character->SetRotation(0.0f, 0.0f, 0.0f);
	character->SetScale(1.0f, 1.0f, 1.0f);

	remotePlayer->GetInventory()->UpdateMoney(amount);

	{
		TaskManagerPtr taskManager = character->GetTaskManagerRef().lock();
		EqipmentComponent& eqipment = character->GetEqipmentComponent();
		eqipment.CreateEqipment(taskManager, helmet,	1);
		eqipment.CreateEqipment(taskManager, shoulders, 2);
		eqipment.CreateEqipment(taskManager, chest,		3);
		eqipment.CreateEqipment(taskManager, bracers,	4);
		eqipment.CreateEqipment(taskManager, hands,		5);
		eqipment.CreateEqipment(taskManager, pants,		6);
		eqipment.CreateEqipment(taskManager, boots,		7);
		eqipment.CreateEqipment(taskManager, weapon_l,	8);
		eqipment.CreateEqipment(taskManager, weapon_r,	9);
	}
	
	//TODO : 캐릭터와 장비스텟 나눠야 할 수도..?
	character->GetStatComponent().InitMaxStats(character->GetActorPtr(), EGameDataType::BaseStat, EGameDataType::GrowStat, characterClass - 1, level);

	character->SetLoadCharacter(true);

	remotePlayer->OnLoadComplete();
	return true;
}
