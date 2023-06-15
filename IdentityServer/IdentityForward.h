#pragma once

class IdentityService;
class IdentityListener;
class IdentityGameState;
class IdentityPlayerState;
class IdentityDatabase;
class IdentitiyDatabaseHandler;
class IdentityDataManager;
class IdentityTask;

using IdentityServicePtr			= std::shared_ptr<IdentityService>;
using IdentityListenerPtr			= std::shared_ptr<IdentityListener>;
using GameStatePtr					= std::shared_ptr<IdentityGameState>;
using PlayerStatePtr				= std::shared_ptr<IdentityPlayerState>;
using IdentityDatabasePtr			= std::shared_ptr<IdentityDatabase>;
using IdentitiyDatabaseHandlerPtr	= std::shared_ptr<IdentitiyDatabaseHandler>;
using IdentityDataManagerPtr		= std::shared_ptr<IdentityDataManager>;
using IdentityTaskPtr				= std::shared_ptr<IdentityTask>;

class World;
class LoginRoom;
class SelectRoom;
class CustomRoom;
class RemotePlayer;
class Character;

using WorldPtr						= std::shared_ptr<World>;
using WorldRef						= std::weak_ptr<World>;
using LoginRoomPtr					= std::shared_ptr<LoginRoom>;
using SelectRoomPtr					= std::shared_ptr<SelectRoom>;
using CustomRoomPtr					= std::shared_ptr<CustomRoom>;
using RemotePlayerPtr				= std::shared_ptr<RemotePlayer>;
using CharacterPtr					= std::shared_ptr<Character>;

