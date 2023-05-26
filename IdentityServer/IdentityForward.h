#pragma once

class IdentityService;
class IdentityListener;
class IdentityGameState;
class IdentityPlayerState;
class IdentityDatabase;
class IdentitiyDatabaseHandler;

using IdentityServicePtr = std::shared_ptr<IdentityService>;
using IdentityListenerPtr = std::shared_ptr<IdentityListener>;
using GameStatePtr = std::shared_ptr<IdentityGameState>;
using PlayerStatePtr = std::shared_ptr<IdentityPlayerState>;
using IdentityDatabasePtr = std::shared_ptr<IdentityDatabase>;
using IdentitiyDatabaseHandlerPtr = std::shared_ptr<IdentitiyDatabaseHandler>;

class LoginRoom;
class RemotePlayer;
class Character;

using LoginRoomPtr = std::shared_ptr<LoginRoom>;
using RemotePlayerPtr = std::shared_ptr<RemotePlayer>;

