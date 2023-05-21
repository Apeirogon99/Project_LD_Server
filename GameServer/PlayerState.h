#pragma once
class PlayerState : public PacketSession
{
public:
	PlayerState();
	virtual ~PlayerState();

	PlayerState(const PlayerState& player) = delete;
	PlayerState& operator=(const PlayerState& player) = delete;

protected:
	virtual void OnConnected() override;
	virtual void OnSend(uint32 len) override;
	virtual void OnIcmp() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, const uint32 len) override;

public:
	template <typename... Types>
	void PlayerStateLog(const WCHAR* inLog, Types... inArgs)
	{
		const int64 remoteID = 0;

		std::wstring tempLog;
		tempLog.append(L"[REMOTE ID::]");
		tempLog.append(std::to_wstring(remoteID));
		tempLog.append(L"] ");
		tempLog.append(inLog);

		this->SessionLog(tempLog.c_str(), inArgs...);
	}

};

