#pragma once
class LoginRoom : public NetworkQueue
{
public:
	void Enter(RemotePlayerPtr inRemotePlayer);
	void Leave(RemotePlayerPtr inRemotePlayer);
	void Deliver(RemotePlayerPtr inRemotePlayer);
	void Broadcast(RemotePlayerPtr inRemotePlayer);

private:
	std::map<uint64, RemotePlayerPtr> mRemotePlayers;
};

