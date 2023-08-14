#pragma once

using ItemHandlerFunc = std::function<bool(GameRemotePlayerRef&)>;

enum class EItemID : uint16
{
	Item_Position = 1,
};

bool Item_INVALID(GameRemotePlayerRef& inGameRemotePlayer);
bool Item_Position(GameRemotePlayerRef& inGameRemotePlayer);

class ItemHandler
{
public:
	static void Init()
	{
		for (uint16 i = 0; i < UINT16_MAX; ++i)
		{
			mItemPacketHandlers[i] = Item_INVALID;
		}

		mItemPacketHandlers[static_cast<uint16>(EItemID::Item_Position)] = [](GameRemotePlayerRef& inGameRemotePlayer) { return ItemHandler::HandlePacket(Item_Position, inGameRemotePlayer); };
	}

	static bool HandlePacket(GameRemotePlayerRef& inGameRemotePlayer, int64 inItemID)
	{
		return mItemPacketHandlers[inItemID](inGameRemotePlayer);
	}

private:
	template<typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, GameRemotePlayerRef& inGameRemotePlayer)
	{
		return func(inGameRemotePlayer);
	}

public:
	static ItemHandlerFunc mItemPacketHandlers[UINT16_MAX];
};

