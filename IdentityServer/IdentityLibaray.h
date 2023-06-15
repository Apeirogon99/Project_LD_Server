#pragma once

#define NETWORK_LOCAL 1

//==========================//
//		     TYPE			//
//==========================//
#include "IdentityForward.h"
#include "FowradEnum.h"

#include "IdentityPacket.pb.h"
#include "CommonPacket.pb.h"

//==========================//
//		 GameContent		//
//==========================//
#include "Character.h"
#include "RemotePlayer.h"
#include "World.h"
#include "LoginRoom.h"
#include "SelectRoom.h"
#include "CustomRoom.h"

//==========================//
//		   Framework		//
//==========================//
#include "IdentityService.h"
#include "IdentityGameState.h"
#include "IdentityPlayerState.h"
#include "IdentityListener.h"
#include "IdentityDatabase.h"
#include "IdentityDataManager.h"
#include "IdentityTask.h"

//==========================//
//		    Packet			//
//==========================//
#include "PacketHandler.h"
#include "IdentityServerPacketHandler.h"
#include "CommonServerPacketHandler.h"

//==========================//
//		     DB				//
//==========================//
#include "DatabaseHandler.h"
#include "IdentitiyDatabase.h"
#include "CharacterDatabase.h"