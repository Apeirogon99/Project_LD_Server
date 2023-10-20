#include "pch.h"
#include "ItemHandler.h"

ItemHandlerFunc ItemHandler::mItemPacketHandlers[UINT16_MAX];

bool Item_INVALID(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    return false;
}

bool Item_Health_Position(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    if (false == inIsPressed)
    {
        return true;
    }

    GameRemotePlayerPtr remotePlayer = inGameRemotePlayer.lock();

    GameWorldPtr world = std::static_pointer_cast<GameWorld>(remotePlayer->GetWorld().lock());
    if (nullptr == world)
    {
        return false;
    }

    PlayerCharacterPtr character = remotePlayer->GetCharacter();
    if (nullptr == character)
    {
        return false;
    }

    return true;
}

bool Item_Mana_Position(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    return true;
}
