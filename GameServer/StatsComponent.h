#pragma once

class StatsComponent
{
public:
	StatsComponent();
	~StatsComponent();

	StatsComponent(const StatsComponent&) = delete;
	StatsComponent(StatsComponent&&) noexcept = delete;

	StatsComponent& operator=(const StatsComponent&) = delete;
	StatsComponent& operator=(StatsComponent&&) noexcept = delete;

public:
	void SetSyncTime(const int64 inSyncTime);
	void InitMaxStats(const Stats& inMaxStats);
	void InitMaxStats(ActorPtr inActor, const EGameDataType inBaseData, int32 inClass);
	void InitMaxStats(ActorPtr inActor, const EGameDataType inBaseData, const EGameDataType inGrowData, const int32 inClass, const int32 inLevel);

public:
	bool ExtractDifferentMaxStats(std::map<EStatType, float>& outUpdateStats);
	bool ExtractChanageMaxStats(std::map<EStatType, float>& outUpdateStats);
	void UpdateMaxStats(ActorPtr inActor, const int32 inLevel);
	void UpdateCurrentStat(const EStatType inStatType, const float inValue);

public:
	const Stats&	GetMaxStats() const;
	const Stats&	GetCurrentStats() const;
	bool			IsChanageStats(const int64 inDeletaTime);

private:
	bool			mIsChanageStats;
	Stats			mMaxStats;
	Stats			mCurrentStats;

	int32			mClass;
	EGameDataType	mBaseType;
	EGameDataType	mGrowType;

	int64			mStatSyncTime;
	int64			mMaxStatSyncTime;

	std::map<EStatType, float> mUpdateStats;
};

