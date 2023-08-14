#include "pch.h"
#include "ItemHandler.h"

ItemHandlerFunc ItemHandler::mItemPacketHandlers[UINT16_MAX];

bool Item_INVALID(GameRemotePlayerRef& inGameRemotePlayer)
{
    return false;
}

bool Item_Position(GameRemotePlayerRef& inGameRemotePlayer)
{
    return false;
}
