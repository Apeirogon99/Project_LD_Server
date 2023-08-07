#include "pch.h"
#include "LevelComponent.h"

LevelComponent::LevelComponent() : mLevel(0), mCurrentExperience(0), mNextExperience(0)
{
}

LevelComponent::~LevelComponent()
{
}

void LevelComponent::Init(PlayerCharacterRef inOwner, const int32& inLevel, const int32& inCurrentExperience)
{
	mOwner = inOwner;
	mLevel = inLevel;
	mCurrentExperience = inCurrentExperience;

	PlayerCharacterPtr player = inOwner.lock();
	if (nullptr == player)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(player->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	LoadNextExperience(world, inLevel);
}

void LevelComponent::AddExperience(const int32& inNextExperience)
{
	this->mNextExperience += inNextExperience;

	PlayerCharacterPtr player = mOwner.lock();
	if (nullptr == player)
	{
		return;
	}

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(player->GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}

	if (true == IsNextLevel())
	{
		this->mLevel += 1;
		LoadNextExperience(world, mLevel);
	}

	GameRemotePlayerPtr remotePlayer = std::static_pointer_cast<GameRemotePlayer>(player->GetOwner().lock());
	if (nullptr == remotePlayer)
	{
		return;
	}

	PacketSessionPtr session = std::static_pointer_cast<PacketSession>(remotePlayer->GetRemoteClient().lock());
	if (nullptr == session)
	{
		return;
	}

	const int32 characterID = remotePlayer->GetToken().GetCharacterID();
	Handle_Update_Experience_Request(session, characterID, this->mLevel, this->mCurrentExperience);
}

const bool LevelComponent::IsNextLevel() const
{
	if (mNextExperience >= mCurrentExperience)
	{
		return false;
	}

	return true;
}

const int32& LevelComponent::GetLevel() const
{
	return mLevel;
}

const int32& LevelComponent::GetCurrentExperience() const
{
	return mCurrentExperience;
}

const int32& LevelComponent::GetNextExperience() const
{
	return mNextExperience;
}

void LevelComponent::LoadNextExperience(GameWorldPtr inGameWorld, const int32& inLevel)
{
	GameDatasPtr data = std::static_pointer_cast<GameDatas>(inGameWorld->GetDatas());
	if (nullptr == data)
	{
		return;
	}

	this->mNextExperience = data->GetNextExperience(inLevel + 1);
	this->mCurrentExperience = this->mCurrentExperience % this->mNextExperience;
}
