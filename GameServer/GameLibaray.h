#pragma once

#define NETWORK_LOCAL 1

//==========================//
//		     TYPE			//
//==========================//
#include "GameFoward.h"
#include "Mecro.h"

#include "CommonPacket.pb.h"
#include "GamePacket.pb.h"
#include "PacketEnum.pb.h"
#include "PacketStruct.pb.h"

//==========================//
//		   Framework		//
//==========================//
#include "GameService.h"
#include "GameState.h"
#include "PlayerState.h"
#include "GameListener.h"
#include "GameDatabase.h"
#include "GameDatas.h"

//==========================//
//		 GameContent		//
//==========================//
#include "Actor.h"
#include "AItem.h"
#include "Inventory.h"
#include "Character.h"
#include "RemotePlayer.h"
#include "World.h"

//==========================//
//		    Packet			//
//==========================//
#include "PacketHandler.h"
#include "GameServerPacketHandler.h"
#include "CommonServerPacketHandler.h"

//==========================//
//		   DataBase			//
//==========================//
#include "GameDataBaseHandler.h"
#include "InventoryDatabase.h"
