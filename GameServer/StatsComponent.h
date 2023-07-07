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
	void InitMaxStats(const Stats& inMaxStats);
	void InitMaxStats(ActorPtr inActor, const EGameDataType inBaseData, int32 inClass);
	void InitMaxStats(ActorPtr inActor, const EGameDataType inBaseData, const EGameDataType inGrowData, const int32 inClass, const int32 inLevel);

public:
	void UpdateMaxStats(ActorPtr inActor, const int32 inLevel);
	void UpdateCurrentStat(const EStatType inStatType, const float inValue);

public:
	const Stats& GetMaxStats();
	const Stats& GetCurrentStats();

private:
	Stats			mMaxStats;
	Stats			mCurrentStats;

	int32			mClass;
	EGameDataType	mBaseType;
	EGameDataType	mGrowType;
};

