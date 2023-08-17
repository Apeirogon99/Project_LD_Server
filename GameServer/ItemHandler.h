#pragma once

using ItemHandlerFunc = std::function<bool(GameRemotePlayerRef&, bool)>;

enum class EItemID : uint16
{
	Item_Position = 1,
};

bool Item_INVALID(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed);
bool Item_Position(GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed);

class ItemHandler
{
public:
	static void Init()
	{
		for (uint16 i = 0; i < UINT16_MAX; ++i)
		{
			mItemPacketHandlers[i] = Item_INVALID;
		}

		mItemPacketHandlers[static_cast<uint16>(EItemID::Item_Position)] = [](GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed) { return ItemHandler::HandlePacket(Item_Position, inGameRemotePlayer, inIsPressed); };
	}

	static bool HandlePacket(GameRemotePlayerRef& inGameRemotePlayer, int64 inItemID, bool inIsPressed)
	{
		return mItemPacketHandlers[inItemID](inGameRemotePlayer, inIsPressed);
	}

private:
	template<typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, GameRemotePlayerRef& inGameRemotePlayer, bool inIsPressed)
	{
		return func(inGameRemotePlayer, inIsPressed);
	}

public:
	static ItemHandlerFunc mItemPacketHandlers[UINT16_MAX];
};

