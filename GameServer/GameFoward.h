#pragma once

//==========================//
//		   Framework		//
//==========================//
class GameDatabase;
class GameState;
class GameListener;
class PlayerState;
class GameService;

using GameDatabasePtr	= std::shared_ptr<GameDatabase>;
using GameStatePtr		= std::shared_ptr<GameState>;
using GameListenerPtr	= std::shared_ptr<GameListener>;
using PlayerStatePtr	= std::shared_ptr<PlayerState>;
using GameServicePtr	= std::shared_ptr<GameService>;

//==========================//
//		    Packet			//
//==========================//

//==========================//
//		 GameContent		//
//==========================//