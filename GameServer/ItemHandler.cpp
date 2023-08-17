#include "pch.h"
#include "ItemHandler.h"

ItemHandlerFunc ItemHandler::mItemPacketHandlers[UINT16_MAX];

bool Item_INVALID(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    return false;
}

bool Item_Position(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
{
    return false;
}
