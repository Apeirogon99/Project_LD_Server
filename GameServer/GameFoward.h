#pragma once

//==========================//
//		   Framework		//
//==========================//
class GameDatabase;
class GameState;
class GameListener;
class PlayerState;
class GameService;
class GameDatas;
class GameTask;

using GameDatabasePtr	= std::shared_ptr<GameDatabase>;
using GameStatePtr		= std::shared_ptr<GameState>;
using GameListenerPtr	= std::shared_ptr<GameListener>;
using PlayerStatePtr	= std::shared_ptr<PlayerState>;
using PlayerStateRef	= std::weak_ptr<PlayerState>;
using GameServicePtr	= std::shared_ptr<GameService>;
using GameDatasPtr		= std::shared_ptr<GameDatas>;
using GameTaskPtr		= std::shared_ptr<GameTask>;

//==========================//
//		    Packet			//
//==========================//


//==========================//
//	   Content Framework	//
//==========================//
class GameWorld;
using GameWorldPtr = std::shared_ptr<GameWorld>;
using GameWorldRef = std::weak_ptr<GameWorld>;

//==========================//
//	       Component		//
//==========================//
class StatsComponent;
class EqipmentComponent;
class AttackComponent;

//==========================//
//		 GameContent		//
//==========================//


//==========================//
//   GameContent | Item		//
//==========================//
class AItem;
using AItemPtr						= std::shared_ptr<AItem>;
using AItemRef						= std::weak_ptr<AItem>;

//==========================//
//   GameContent | Player	//
//==========================//
class GameRemotePlayer;
using GameRemotePlayerPtr			= std::shared_ptr<GameRemotePlayer>;
using GameRemotePlayerRef			= std::weak_ptr<GameRemotePlayer>;

class PlayerCharacter;
using PlayerCharacterPtr			= std::shared_ptr<PlayerCharacter>;
using PlayerCharacterRef			= std::weak_ptr<PlayerCharacter>;

class Inventory;
using Inventoryptr					= std::shared_ptr<Inventory>;
using InventoryRef					= std::weak_ptr<Inventory>;

//==========================//
//  GameContent | Projctile //
//==========================//
class Arrow;

using ArrowPtr						= std::shared_ptr<Arrow>;
using ArrowRef						= std::weak_ptr<Arrow>;

//==========================//
//   GameContent | Enemy	//
//==========================//
class EnemyCharacter;
class EnemySpawner;
class EnemySpawnerManager;
class EnemySlime;
class StateManager;

using EnemyCharacterPtr		= std::shared_ptr<EnemyCharacter>;
using EnemyCharacterRef		= std::weak_ptr<EnemyCharacter>;

using EnemySpawnerPtr		= std::shared_ptr<EnemySpawner>;
using EnemySpawnerRef		= std::weak_ptr<EnemySpawner>;

using EnemySpawnerManagerPtr = std::shared_ptr<EnemySpawnerManager>;
using EnemySpawnerManagerRef = std::weak_ptr<EnemySpawnerManager>;

using EnemySlimePtr			= std::shared_ptr<EnemySlime>;

//==========================//
//		    USING			//
//==========================//



//==========================//
//		    Mecro			//
//==========================//