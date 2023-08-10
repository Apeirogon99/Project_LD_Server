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
//   GameContent | Componet	//
//==========================//
#include "StatsComponent.h"
#include "EqipmentComponent.h"
#include "AttackComponent.h"
#include "LevelComponent.h"

//==========================//
//		 GameContent		//
//==========================//
#include "GameWorld.h"
#include "GameRemotePlayer.h"
#include "PlayerCharacter.h"
#include "NPCCharacter.h"
#include "AItem.h"
#include "Inventory.h"
#include "Friend.h"
#include "Skill.h"

//==========================//
//   GameContent | Enemy	//
//==========================//
#include "IStateEvent.h"
#include "Arrow.h"
#include "EnemyCharacter.h"
#include "EnemySpawner.h"
#include "EnemySlime.h"
#include "EnemyNomalSkeleton.h"
#include "EnemyArcherSkeleton.h"
#include "EnemyWarriorSkeleton.h"

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
#include "friendDatabase.h"
#include "SkillDatabase.h"
