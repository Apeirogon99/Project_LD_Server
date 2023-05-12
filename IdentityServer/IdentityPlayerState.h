#pragma once

class IdentityPlayerState : public PacketSession
{
public:
	IdentityPlayerState();
	virtual ~IdentityPlayerState();

	IdentityPlayerState(const IdentityPlayerState& player) = delete;
	IdentityPlayerState& operator=(const IdentityPlayerState& player) = delete;

protected:
	virtual void OnConnected() override;
	virtual void OnSend(uint32 len) override;
	virtual void OnIcmp() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, const uint32 len) override;

public:
	void PlayerStateLog(const WCHAR* log, ...);

public:
	RemotePlayerPtr mRemotePlayer;
};

