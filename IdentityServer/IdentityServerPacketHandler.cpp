#include "pch.h"
#include "IdentityServerPacketHandler.h"

bool Handle_INVALID(PacketSessionPtr& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_C2S_EnterIdentityServer(PacketSessionPtr& session, Protocol::C2S_EnterIdentityServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	//리모트 아이디 제공?

	return false;
}

bool Handle_C2S_LeaveIdentityServer(PacketSessionPtr& session, Protocol::C2S_LeaveIdentityServer& pkt)
{
	//여기에서 세션을 제거
	return false;
}

bool Handle_C2S_Singin(PacketSessionPtr& session, Protocol::C2S_Singin& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	Handle_Singin_Requset(session, pkt);

	return true;
}

bool Handle_C2S_Singup(PacketSessionPtr& session, Protocol::C2S_Singup& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"account_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection conn;
	conn.Open(ConnectionInfo);

	std::string tempEmail = pkt.user_email();
	size_t find = tempEmail.find('@');

	ADOVariant userID = pkt.user_id().c_str();
	ADOVariant userPS = pkt.user_password().c_str();
	ADOVariant local = tempEmail.substr(0, find).c_str();
	ADOVariant domain = tempEmail.substr(find + 1, tempEmail.length()).c_str();

	ADOCommand cmd;
	cmd.SetStoredProcedure(conn, L"dbo.singup_sp");
	cmd.SetReturnParam();
	cmd.SetInputParam(L"@name", userID);
	cmd.SetInputParam(L"@password", userPS);
	cmd.SetInputParam(L"@local", local);
	cmd.SetInputParam(L"@domain", domain);
	cmd.SetOutputParam(L"@global_id", DataTypeEnum::adInteger, sizeof(int32));

	ADORecordset rs;
	cmd.ExecuteStoredProcedure(rs, EExcuteReturnType::Wait_Return);

	
	Protocol::S2C_Singup singupPacket;
	int32 ret = cmd.GetReturnParam();
	if (ret != 0)
	{
		singupPacket.set_error(ret);
	}
	else
	{
		int32 globalID = cmd.GetOutputParam(L"@global_id");
		int64 remoteID = playerState->mRemoteID;

		playerState->mGlobalID = globalID;
		singupPacket.set_error(ret);
		singupPacket.set_remote_id(remoteID);
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, singupPacket);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C2S_EmailVerified(PacketSessionPtr& session, Protocol::C2S_EmailVerified& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"account_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection conn;
	conn.Open(ConnectionInfo);

	ADOVariant global_id = playerState->mGlobalID;
	if (static_cast<int32>(global_id) > -1)
	{
		return false;
	}
	ADOVariant verifyCode = atoi(pkt.verified_code().c_str());

	ADOCommand cmd;
	cmd.SetStoredProcedure(conn, L"dbo.check_email_verify_sp");
	cmd.SetReturnParam();
	cmd.SetInputParam(L"@global_id", global_id);
	cmd.SetInputParam(L"@code", verifyCode);

	ADORecordset rs;
	cmd.ExecuteStoredProcedure(rs, EExcuteReturnType::Wait_Return);

	Protocol::S2C_EmailVerified emailVerifiedPacket;
	int32 ret = cmd.GetReturnParam();

	int64 remoteID = playerState->mRemoteID;
	emailVerifiedPacket.set_error(ret);
	emailVerifiedPacket.set_remote_id(remoteID);

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, emailVerifiedPacket);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C2S_LoadCharacters(PacketSessionPtr& session, Protocol::C2S_LoadCharacters& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection conn;
	conn.Open(ConnectionInfo);

	ADOVariant global_id = playerState->mGlobalID;
	ADOVariant server_id = pkt.server_id();

	ADOCommand cmd;
	cmd.SetStoredProcedure(conn, L"dbo.load_character_sp");
	cmd.SetReturnParam();
	cmd.SetInputParam(L"@global_id", global_id);
	cmd.SetInputParam(L"@server_id", server_id);

	ADORecordset rs;
	cmd.ExecuteStoredProcedure(rs, EExcuteReturnType::Wait_Return);


	Protocol::S2C_LoadCharacters loadCharacterPackets;
	int32 loadCharacterPacketIndex = 0;
	if (rs.IsOpen())
	{
		while (!(rs.IsEof()))
		{
			std::string name			= rs.GetFieldItem(L"name");
			int32		characterClass	= rs.GetFieldItem(L"character_calss_id");
			int32		race			= rs.GetFieldItem(L"race_id");
			//int32		level			= rs.GetFieldItem(L"level");
			int32		seat			= rs.GetFieldItem(L"seat");
			int32		skin_color		= rs.GetFieldItem(L"skin_color");
			int32		hair_color		= rs.GetFieldItem(L"hair_color");
			int32		eye_color		= rs.GetFieldItem(L"eye_color");
			int32		eyebrow_color	= rs.GetFieldItem(L"eyebrow_color");

			//int32		ears			= rs.GetFieldItem(L"ears");
			int32		hair			= rs.GetFieldItem(L"hair");
			//int32		facials_01		= rs.GetFieldItem(L"facials_01");
			//int32		facials_02		= rs.GetFieldItem(L"facials_02");
			int32		helmet			= rs.GetFieldItem(L"helmet");
			int32		shoulders		= rs.GetFieldItem(L"shoulders");
			//int32		skirt			= rs.GetFieldItem(L"skirt");
			int32		pants_add		= rs.GetFieldItem(L"pants_add");
			//int32		legs_add		= rs.GetFieldItem(L"legs_add");
			int32		hands_add		= rs.GetFieldItem(L"hands_add");
			int32		chest_add		= rs.GetFieldItem(L"chest_add");
			//int32		cape			= rs.GetFieldItem(L"cape");
			int32		bracers_add		= rs.GetFieldItem(L"bracers_add");
			int32		boots			= rs.GetFieldItem(L"boots");
			//int32		belt			= rs.GetFieldItem(L"belt");
			//int32		tabard			= rs.GetFieldItem(L"tabard");
			int32		weapon_l		= rs.GetFieldItem(L"weapon_l");
			int32		weapon_r		= rs.GetFieldItem(L"weapon_r");

			std::string* names = loadCharacterPackets.add_name();
			*names = name;

			Protocol::SCharacterAppearance* appearances = loadCharacterPackets.add_appearance();
			appearances->set_race(static_cast<Protocol::ERace>(race));
			appearances->set_character_class(static_cast<Protocol::ECharacterClass>(characterClass));
			appearances->set_seat(seat);
			appearances->set_skin_color(skin_color);
			appearances->set_hair_color(hair_color);
			appearances->set_eye_color(eye_color);
			appearances->set_eyebrow_color(eyebrow_color);

			Protocol::SCharacterEqipment* eqipments = loadCharacterPackets.add_eqipment();
			//eqipments->set_ears(ears);
			eqipments->set_hair(hair);
			//eqipments->set_facials_01(facials_01);
			//eqipments->set_facials_02(facials_02);
			eqipments->set_helmet(helmet);
			eqipments->set_shoulders(shoulders);
			//eqipments->set_skirt(skirt);
			eqipments->set_chest_add(chest_add);
			eqipments->set_bracers_add(bracers_add);
			eqipments->set_hands_add(hands_add);
			eqipments->set_pants_add(pants_add);
			//eqipments->set_legs_add(legs_add);
			//eqipments->set_cape(cape);
			eqipments->set_boots(boots);
			//eqipments->set_belt(belt);
			//eqipments->set_tabard(tabard);
			eqipments->set_weapon_l(weapon_l);
			eqipments->set_weapon_r(weapon_r);

			loadCharacterPacketIndex++;
			rs.MoveNext();
		}
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, loadCharacterPackets);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C2S_CreateCharacter(PacketSessionPtr& session, Protocol::C2S_CreateCharacter& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOVariant name					= pkt.name().c_str();
	ADOVariant characterClass		= pkt.appearance().character_class();
	ADOVariant seat					= pkt.appearance().seat();
	ADOVariant race					= pkt.appearance().race();
	ADOVariant global_id			= playerState->mGlobalID;
	ADOVariant server_id			= pkt.server_id();

	ADOVariant skin_color			= pkt.appearance().skin_color();
	ADOVariant hair_color			= pkt.appearance().hair_color();
	ADOVariant eye_color			= pkt.appearance().eye_color();
	ADOVariant eyebrow_color		= pkt.appearance().eyebrow_color();
	
	//ADOVariant head				= pkt.appearance().head();
	//ADOVariant ears				= pkt.appearance().ears();
	//ADOVariant feet				= pkt.appearance().feet();
	//ADOVariant hair				= pkt.appearance().hair();
	//ADOVariant facials_01			= pkt.appearance().facials_01();
	//ADOVariant facials_02			= pkt.appearance().facials_02();
	//ADOVariant helmet				= pkt.appearance().helmet();
	//ADOVariant shoulders			= pkt.appearance().shoulders();
	//ADOVariant skirt				= pkt.appearance().skirt();
	//ADOVariant legs				= pkt.appearance().legs();
	//ADOVariant legs_add			= pkt.appearance().legs_add();
	//ADOVariant hands				= pkt.appearance().hands();
	//ADOVariant hands_add			= pkt.appearance().hands_add();
	//ADOVariant chest				= pkt.appearance().chest();
	//ADOVariant chest_add			= pkt.appearance().chest_add();
	//ADOVariant cape				= pkt.appearance().cape();
	//ADOVariant bracers			= pkt.appearance().bracers();
	//ADOVariant bracers_add		= pkt.appearance().bracers_add();
	//ADOVariant boots				= pkt.appearance().boots();
	//ADOVariant belt				= pkt.appearance().belt();
	//ADOVariant tabard				= pkt.appearance().tabard();
	//ADOVariant back_2hl			= pkt.appearance().back_2hl();
	//ADOVariant back_shield		= pkt.appearance().back_shield();
	//ADOVariant back_weapon_l		= pkt.appearance().back_weapon_l();
	//ADOVariant back_weapon_r		= pkt.appearance().back_weapon_r();
	//ADOVariant back_bow			= pkt.appearance().back_bow();
	//ADOVariant quiver				= pkt.appearance().quiver();
	//ADOVariant weapon_r_arrow		= pkt.appearance().weapon_r_arrow();
	//ADOVariant weapon_shield		= pkt.appearance().weapon_shield();
	//ADOVariant weapon_l			= pkt.appearance().weapon_l();
	//ADOVariant weapon_r			= pkt.appearance().weapon_r();
	//ADOVariant hip_l				= pkt.appearance().hip_l();
	//ADOVariant hip_r				= pkt.appearance().hip_r();


	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection conn;
	conn.Open(ConnectionInfo);

	ADOCommand cmd;
	cmd.SetStoredProcedure(conn, L"dbo.create_character_sp");
	cmd.SetReturnParam();
	cmd.SetInputParam(L"@name", name);
	cmd.SetInputParam(L"@class", characterClass);
	cmd.SetInputParam(L"@race", race);
	cmd.SetInputParam(L"@seat", seat);
	cmd.SetInputParam(L"@global_id", global_id);
	cmd.SetInputParam(L"@server_id", server_id);

	cmd.SetInputParam(L"@skin_color", skin_color);
	cmd.SetInputParam(L"@hair_color", hair_color);
	cmd.SetInputParam(L"@eye_color", eye_color);
	cmd.SetInputParam(L"@eyebrow_color", eyebrow_color);

	//cmd.SetInputParam(L"@head", head);
	//cmd.SetInputParam(L"@ears", ears);
	//cmd.SetInputParam(L"@feet", feet);
	//cmd.SetInputParam(L"@hair", hair);
	//cmd.SetInputParam(L"@facials_02", facials_02);
	//cmd.SetInputParam(L"@legs", legs);
	//cmd.SetInputParam(L"@hands", hands);
	//cmd.SetInputParam(L"@chest", chest);
	//cmd.SetInputParam(L"@bracers", bracers);
	//cmd.SetInputParam(L"@boots	", boots);

	ADORecordset rs;
	cmd.ExecuteStoredProcedure(rs, EExcuteReturnType::Wait_Return);

	Protocol::S2C_CreateCharacter createCharacterPacket;
	int32 ret = cmd.GetReturnParam();
	if (ret != 0)
	{
		createCharacterPacket.set_error(ret);
	}
	else
	{
		createCharacterPacket.set_error(ret);
	}

	SendBufferPtr sendBuffer = IdentityServerPacketHandler::MakeSendBuffer(session, createCharacterPacket);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C2S_UpdateAppearance(PacketSessionPtr& session, Protocol::C2S_UpdateAppearance& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	ADOConnectionInfo ConnectionInfo(L"SQLOLEDB", L"APEIROGON", L"game_database", L"SSPI", L"NO", L"apeirogon", L"1248", EDBMSTypes::MSSQL);
	ADOConnection conn;
	conn.Open(ConnectionInfo);

	ADOVariant characterID = 1;
	ADOVariant skin_color = pkt.appearance().skin_color();
	ADOVariant hair_color = pkt.appearance().hair_color();
	ADOVariant eye_color = pkt.appearance().eye_color();
	ADOVariant eyebrow_color = pkt.appearance().eyebrow_color();

	ADOCommand cmd;
	cmd.SetStoredProcedure(conn, L"dbo.update_appearance_sp");
	cmd.SetReturnParam();
	cmd.SetInputParam(L"@character_id", characterID);
	cmd.SetInputParam(L"@skin_color", skin_color);
	cmd.SetInputParam(L"@hair_color", hair_color);
	cmd.SetInputParam(L"@eye_color", eye_color);
	cmd.SetInputParam(L"@eyebrow_color", eyebrow_color);

	ADORecordset rs;
	cmd.ExecuteStoredProcedure(rs, EExcuteReturnType::Wait_Return);

	return true;
}

bool Handle_C2S_DeleteCharacter(PacketSessionPtr& session, Protocol::C2S_DeleteCharacter& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_UpdateNickName(PacketSessionPtr& session, Protocol::C2S_UpdateNickName& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	return true;
}

bool Handle_C2S_TravelServer(PacketSessionPtr& session, Protocol::C2S_TravelServer& pkt)
{
	PlayerStatePtr playerState = std::static_pointer_cast<IdentityPlayerState>(session);
	bool valid = playerState->IsValid();
	if (false == valid)
	{
		return false;
	}

	return true;
}