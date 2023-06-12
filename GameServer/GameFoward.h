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
class Actor;
class Pawn;
class Character;
class AICharacter;
class RemotePlayer;
class RemoteClient;

using ActorPtr			= std::shared_ptr<Actor>;
using PawnPtr			= std::shared_ptr<Pawn>;
using CharacterPtr		= std::shared_ptr<Character>;
using AICharacterPtr	= std::shared_ptr<AICharacter>;
using RemotePlayerPtr	= std::shared_ptr<RemotePlayer>;
using RemotePlayerRef	= std::weak_ptr<RemotePlayer>;
using RemoteClientPtr	= std::weak_ptr<RemoteClient>;


//==========================//
//		 GameContent		//
//==========================//
class World;
class AItem;
class Inventory;

using WorldPtr = std::shared_ptr<World>;
using WorldRef = std::weak_ptr<World>;
using AItemPtr = std::shared_ptr<AItem>;
using Inventoryptr = std::shared_ptr<Inventory>;

//==========================//
//		    USING			//
//==========================//

using Viewers			= std::set<PlayerStatePtr>;
using Monitors			= std::set<RemotePlayerPtr>;