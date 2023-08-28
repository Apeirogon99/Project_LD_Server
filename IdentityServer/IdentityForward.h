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

class IdentityManager;
using IdentityManagerPtr		= std::shared_ptr<IdentityManager>;
using IdentityManagerRef		= std::weak_ptr<IdentityManager>;

class LoginWorld;
using LoginWorldPtr				= std::shared_ptr<LoginWorld>;
using LoginWorldRef				= std::weak_ptr<LoginWorld>;

class RoomManager;
using RoomManagerPtr			= std::shared_ptr<RoomManager>;
using RoomManagerRef			= std::weak_ptr<RoomManager>;

class LoginRoom;
using LoginRoomPtr				= std::shared_ptr<LoginRoom>;
using LoginRoomRef				= std::weak_ptr<LoginRoom>;

class SelectRoom;
using SelectRoomPtr				= std::shared_ptr<SelectRoom>;
using SelectRoomRef				= std::weak_ptr<SelectRoom>;

class CustomRoom;
using CustomRoomPtr				= std::shared_ptr<CustomRoom>;
using CustomRoomRef				= std::weak_ptr<CustomRoom>;

class LoginRemotePlayer;
using LoginRemotePlayerPtr		= std::shared_ptr<LoginRemotePlayer>;
using LoginRemotePlayerRef		= std::weak_ptr<LoginRemotePlayer>;

class CharacterManager;
using CharacterManagerPtr		= std::shared_ptr<CharacterManager>;
using CharacterManagerRef		= std::weak_ptr<CharacterManager>;

class LoginCharacter;
using LoginCharacterPtr			= std::shared_ptr<LoginCharacter>;
using LoginCharacterRef			= std::weak_ptr<LoginCharacter>;

