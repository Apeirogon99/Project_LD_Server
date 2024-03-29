#pragma once

class SkillTreeInfo
{
public:
	SkillTreeInfo() : mID(0), mType(0), mMax(0)
	{
	}

	~SkillTreeInfo()
	{
	}

	void Init(const int32& inID, const int32& inType, const int32& inMax, const std::map<int32, int32>& inConditions)
	{
		this->mID = inID;
		this->mType = mType;
		this->mMax = inMax;
		this->mConditions = inConditions;
	}

	void Clear()
	{
		this->mID	= 0;
		this->mType = 0;
		this->mMax	= 0;
		mConditions.clear();
	}

public:
	const int32& GetID()		const { return mID; }
	const int32& GetType()		const { return mType; }
	const int32& GetMaxCount()	const { return mMax; }
	const std::map<int32, int32>& GetConditions()	const { return mConditions; }

private:
	int32 mID;
	int32 mType;
	int32 mMax;
	std::map<int32, int32> mConditions;
};

class SkillTree : public GameObject
{
public:
	SkillTree();
	virtual ~SkillTree();

	SkillTree(const SkillTree&) = delete;
	SkillTree(SkillTree&&) noexcept = delete;

	SkillTree& operator=(const SkillTree&) = delete;
	SkillTree& operator=(SkillTree&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void SetLoadSkillTree(const bool& inLoad);

	void LoadSkillTree(const int32& inSkillID, const int32& inSkillCount);
	void UpdateSkillTree(const int32 inSkillID, const int32 inSkillCount);
	void RollbackSkillTree(const int32& inSkillID, const int32& inSkillCount);

public:
	bool CanUpdateSkill(const int32 inSkillID);

private:
	bool mIsLoad;
	std::map<int32, int32> mSkillTrees;
};

