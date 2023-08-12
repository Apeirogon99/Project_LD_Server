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
	void CreateParty();
	void RequestEnterParty(const std::string inPlayerName);
	void RequestLeaveParty(const int64 inLeaveRemoteID);
	void RequestLeaderParty(const int64 inLeaderRemoteID);
	void ResponseEnterParty(const int64 inResponeRemoteID, const int32 inAction);

	void PartyChat(PlayerStatePtr inPlayerState, const int64 inWorldTime, std::string inMessage);
	void PartyBroadCast(SendBufferPtr inSendBuffer);

	void LoadParty(Protocol::S2C_LoadParty& inLoadPartyPacket);
	void EnterParty(const int64& inRemoteID, GameRemotePlayerPtr inRemotePlayer);
	void LeaveParty(const int64& inRemoteID);

public:
	bool IsParty() const;
	bool IsFull() const;

private:
	int64 mLeaderPlayer;
	std::map<int64, PlayerCharacterPtr> mPlayers;
};

