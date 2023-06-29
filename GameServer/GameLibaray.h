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

#include "PacketUtils.h"

//==========================//
//		   Framework		//
//==========================//
#include "GameService.h"
#include "GameState.h"
#include "PlayerState.h"
#include "GameListener.h"
#include "GameDatabase.h"
#include "GameDatas.h"
#include "GameTask.h"

//==========================//
//		 GameContent		//
//==========================//
#include "Actor.h"
#include "Pawn.h"
#include "Character.h"
#include "NPCCharacter.h"
#include "RemotePlayer.h"
#include "AItem.h"
#include "Inventory.h"
#include "World.h"

//==========================//
//   GameContent | Enemy	//
//==========================//
#include "IStateEvent.h"
#include "EnemyCharacter.h"
#include "EnemySpawner.h"
#include "EnemySlime.h"

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
#include "CharacterDatabase.h"
#include "InventoryDatabase.h"
