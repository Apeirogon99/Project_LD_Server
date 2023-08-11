#pragma once
class Party : public GameObject
{
public:
	Party();
	virtual ~Party();

	Party(const Party&) = delete;
	Party(Party&&) noexcept = delete;

	Party& operator=(const Party&) = delete;
	Party& operator=(Party&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void CreateParty(const int64 inRemoteID);
	void RequestParty(const std::string inPlayerName);
	void ResponseParty(const std::string inPlayerName, const int32 inAction);

	void PartyChat(PlayerStatePtr inPlayerState, const int64 inWorldTime, std::string inMessage);
	void PartyBroadCast(SendBufferPtr inSendBuffer);

public:
	bool IsParty() const;
	bool IsFull() const;

private:
	std::map<int64, PlayerCharacterPtr> mPlayers;
};

