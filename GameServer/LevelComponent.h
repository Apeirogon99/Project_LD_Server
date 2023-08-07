#pragma once
class LevelComponent
{
public:
	LevelComponent();
	~LevelComponent();

	LevelComponent(const LevelComponent&) = delete;
	LevelComponent(LevelComponent&&) noexcept = delete;

	LevelComponent& operator=(const LevelComponent&) = delete;
	LevelComponent& operator=(LevelComponent&&) noexcept = delete;

public:
	void Init(PlayerCharacterRef inOwner, const int32& inLevel, const int32& inCurrentExperience);
	void AddExperience(const int32& inNextExperience);

public:
	const bool		IsNextLevel() const;
	const int32&	GetLevel() const;
	const int32&	GetCurrentExperience() const;
	const int32&	GetNextExperience() const;

protected:
	void LoadNextExperience(GameWorldPtr inGameWorld, const int32& inLevel);

private:
	PlayerCharacterRef	mOwner;
	int32				mLevel;
	int32				mCurrentExperience;
	int32				mNextExperience;
};

