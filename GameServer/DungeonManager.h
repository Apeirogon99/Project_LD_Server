#pragma once

class DungeonManager : public Actor
{
public:
	DungeonManager();
	virtual ~DungeonManager();

	DungeonManager(const DungeonManager&) = delete;
	DungeonManager(DungeonManager&&) noexcept = delete;

	DungeonManager& operator=(const DungeonManager&) = delete;
	DungeonManager& operator=(DungeonManager&&) noexcept = delete;

protected:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void RequestEnterDungeon(GameRemotePlayerPtr inGameRemotePlayer);

private:
	std::vector<Dungeon> mDungeons;
};

