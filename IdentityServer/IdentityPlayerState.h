#pragma once

class IdentityPlayerState : public RemoteClient
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
	GameStatePtr	GetGameState() { return std::static_pointer_cast<IdentityGameState>(GetSessionManager()); }

protected:
	template <typename... Types>
	void PlayerStateLog(const WCHAR* inLog, Types... inArgs);
};

template<typename ...Types>
inline void IdentityPlayerState::PlayerStateLog(const WCHAR* inLog, Types ...inArgs)
{
	const int64 remoteID = mRemotePlayer->GetGameObjectID();

	std::wstring tempLog;
	tempLog.append(L"[REMOTE ID::]");
	tempLog.append(remoteID);
	tempLog.append(L"] ");
	tempLog.append(inLog);

	this->SessionLog(tempLog.c_str(), inArgs...);
}
