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
	void SetPartyLoad(const bool& inLoad);

	void CreateParty();
	void RequestEnterParty(const std::string inPlayerName);
	void RequestLeaveParty(const int64 inLeaveRemoteID);
	void RequestLeaderParty(const int64 inLeaderRemoteID);
	void ResponseEnterParty(const int64 inResponeRemoteID, const int32 inAction);
	void RequestPartyChat(PlayerStatePtr inPlayerState, const int64 inWorldTime, std::string inMessage);

	void LoadParty(Protocol::S2C_LoadParty& inLoadPartyPacket);
	void EnterParty(const int64& inRemoteID);
	void LeaveParty(const int64& inRemoteID);
	void LeaderParty(const int64& inRemoteID);

	void BroadCastEnterParty(const int64& inRemoteID, PlayerCharacterPtr inPlayerCharacter);
	void BroadCastLeaveParty(const int64& inRemoteID);
	void BroadCastLeavePartyWithKick(const int64& inRemoteID);
	void BroadCastLeavePartyWithLeader(const int64& inRemoteID, const int64& inLeaderRemoteID);
	void BroadCastLeaderParty(const int64& inRemoteID);
	void PartyBroadCast(SendBufferPtr inSendBuffer);

	bool PushPartyPlayer(const int64& inRemoteID, PlayerCharacterPtr inPlayerCharacter);
	bool ReleasePartyPlayer(const int64& inRemoteID);
	bool IsPartyPlayer(const int64& inRemoteID);

	bool PushReservationParty(const int64& inRemoteID);
	bool ReleaseReservationParty(const int64& inRemoteID);
	bool IsReservationParty(const int64& inRemoteID);

	void SetPartyPlayers(const std::map<int64, PlayerCharacterRef>& inPartyPlayers);

public:
	bool IsParty() const;
	bool IsFull() const;

public:
	const std::map<int64, PlayerCharacterRef> GetPartyPlayers() const;
	const std::set<int64> GetReservationPlayers() const;
	const int64& GetLeaderRemoteID() const;

private:
	bool								mLoad;
	int64								mLeaderPlayer;
	std::map<int64, PlayerCharacterRef> mPartyPlayers;
	std::set<int64>						mReservationPlayers;
};

