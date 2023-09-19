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

enum class EActorType
{
	Unspecified,
	Obstruction,
	Visible,
	Player,
	PickUp,
	Enemy,
	EnemyAttack,
};

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

class Friend;
using FriendPtr						= std::shared_ptr<Friend>;
using FriendRef						= std::weak_ptr<Friend>;

class Party;
using PartyPtr						= std::shared_ptr<Party>;
using PartyRef						= std::weak_ptr<Party>;

class SkillTree;
using SkillTreePtr					= std::shared_ptr<SkillTree>;
using SkillTreeRef					= std::weak_ptr<SkillTree>;

class Quest;
using QuestPtr						= std::shared_ptr<Quest>;
using QuestRef						= std::weak_ptr<Quest>;

//==========================//
//  GameContent | Projctile //
//==========================//
class Arrow;
using ArrowPtr						= std::shared_ptr<Arrow>;
using ArrowRef						= std::weak_ptr<Arrow>;

//==========================//
//  GameContent | Skill		//
//==========================//
class ActiveSkill;
using ActiveSkillPtr = std::shared_ptr<ActiveSkill>;
using ActiveSkillRef = std::weak_ptr<ActiveSkill>;



//==========================//
//   GameContent | Enemy	//
//==========================//
class EnemySpawner;
class EnemySpawnerManager;
class StateManager;
class EnemyAttack;

class EnemyCharacter;
class EnemySlime;
class EnemyNomalSkeleton;
class EnemyArcherSkeleton;
class EnemyWarriorSkeleton;
class EnemyDarkKnight;
class EnemyRichPhase1;
class EnemyRichPhase2;
class EnemyRichPhase3;

using EnemyCharacterPtr		= std::shared_ptr<EnemyCharacter>;
using EnemyCharacterRef		= std::weak_ptr<EnemyCharacter>;

using EnemyAttackPtr		= std::shared_ptr<EnemyAttack>;
using EnemyAttackRef		= std::weak_ptr<EnemyAttack>;

using EnemySpawnerPtr		= std::shared_ptr<EnemySpawner>;
using EnemySpawnerRef		= std::weak_ptr<EnemySpawner>;

using EnemySpawnerManagerPtr = std::shared_ptr<EnemySpawnerManager>;
using EnemySpawnerManagerRef = std::weak_ptr<EnemySpawnerManager>;

class Dungeon;
using DungeonPtr = std::shared_ptr<Dungeon>;
using DungeonRef = std::weak_ptr<Dungeon>;

class DungeonManager;
using DungeonManagerPtr = std::shared_ptr<DungeonManager>;
using DungeonManagerRef = std::weak_ptr<DungeonManager>;

using EnemySlimePtr			= std::shared_ptr<EnemySlime>;

//==========================//
//		    USING			//
//==========================//



//==========================//
//		    Mecro			//
//==========================//