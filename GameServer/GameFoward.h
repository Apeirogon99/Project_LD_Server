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
//		 GameContent		//
//==========================//
class Actor;
class AItem;
class Inventory;
class Character;
class RemotePlayer;
class World;

using ActorPtr			= std::shared_ptr<Actor>;
using AItemPtr			= std::shared_ptr<AItem>;
using Inventoryptr		= std::shared_ptr<Inventory>;
using RemotePlayerPtr	= std::shared_ptr<RemotePlayer>;
using RemotePlayerRef	= std::weak_ptr<RemotePlayer>;
using CharacterPtr		= std::shared_ptr<Character>;
using WorldPtr			= std::shared_ptr<World>;
using WorldRef			= std::weak_ptr<World>;