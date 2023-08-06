#pragma once

class Friend : public GameObject
{
public:
	Friend();
	virtual ~Friend();

	Friend(const Friend&) = delete;
	Friend(Friend&&) noexcept = delete;

	Friend& operator=(const Friend&) = delete;
	Friend& operator=(Friend&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void SetLoadFriend(bool inIsLoad);

	void LoadFriendList(int32 inListType);
	void RequestFriend(std::string inFriendName, int32 inAction);
	void BlockFriend(std::string inFriendName, int32 inAction);
	void FindFriend(std::string inFriendName);

	bool IsFriend(const int64& inFriendCharacterID);

	void NotifyConnectToFriend();
	void DelegateOnlineFriend(const int64& inFriendCharacterID, const std::string& inFriendName);

	void NotifyDisConnectToFriend();
	void DelegateOfflineFriend(const int64& inFriendCharacterID, const std::string& inFriendName);

public:
	void PushFriend(const std::set<int64>& inFriends);

private:
	bool mIsLoad;
	std::set<int64> mFriends;
};