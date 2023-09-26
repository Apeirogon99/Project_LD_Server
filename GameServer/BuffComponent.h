#pragma once
class BuffComponent
{
public:
	BuffComponent();
	~BuffComponent();

	BuffComponent(const BuffComponent&) = delete;
	BuffComponent(BuffComponent&&) noexcept = delete;

	BuffComponent& operator=(const BuffComponent&) = delete;
	BuffComponent& operator=(BuffComponent&&) noexcept = delete;

public:
	void Init(GameRemotePlayerRef inOwner);

public:
	void ReapplyBuff(StatsComponent& inStatsComponent);
	void PushBuff(StatsComponent& inStatsComponent, const EBuffType& inBuffID, const EStatType& inStatType, const float& inValue);
	void ReleaseBuff(StatsComponent& inStatsComponent, const EBuffType& inBuffID, const EStatType& inStatType, const float& inValue);

public:


private:
	bool						mIsChanage;
	GameRemotePlayerRef			mOwner;
	std::map<EStatType, float>	mBuffs;
};

