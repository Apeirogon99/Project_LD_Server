#pragma once

enum class EBindActionType
{
	Action_Unspecified	= 0,
	Action_Item			= 1,
	Action_Skill		= 2,
};

class BindAction
{
public:
	BindAction() : mActionType(EBindActionType::Action_Unspecified), mActionID(0) {}
	BindAction(const EBindActionType& inActionType, const int32& inActionID) : mActionType(inActionType), mActionID(inActionID) {}
	~BindAction() {}

public:
	void Init(const EBindActionType& inActionType, const int32& inActionID)
	{
		mActionType = inActionType;
		mActionID = inActionID;
	}

public:
	const EBindActionType& GetActionType() const { return mActionType; }
	const int32& GetActionID() const { return mActionID; }

private:
	EBindActionType mActionType;
	int32 mActionID;
};

class KeyboardComponent
{
public:
	KeyboardComponent();
	~KeyboardComponent();

	KeyboardComponent(const KeyboardComponent&) = delete;
	KeyboardComponent(KeyboardComponent&&) noexcept = delete;

	KeyboardComponent& operator=(const KeyboardComponent&) = delete;
	KeyboardComponent& operator=(KeyboardComponent&&) noexcept = delete;

public:
	void LoadBindKeyAndAction(GameRemotePlayerRef inOwner, const std::map<int32, BindAction>& inKeyActions);
	bool SetBindKeyAndAction(const int32& inKey, const EBindActionType& inActionType, const int32& inActionID);

	void PressedKey(int32 inKey);
	void ReleaseKey(int32 inKey);

public:
	const std::map<int32, BindAction>& GetKeyActions() const;

private:
	GameRemotePlayerRef mOwner;
	std::map<int32, BindAction> mKeyActions;
	std::set<int32> mUseKeys;
};