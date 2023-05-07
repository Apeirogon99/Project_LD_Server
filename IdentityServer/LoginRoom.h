#pragma once
class LoginRoom : public NetworkQueue
{
public:
	void Enter(PlayerStatePtr inPlayerState);
	void Leave(PlayerStatePtr inPlayerState);
	void Deliver(PlayerStatePtr inPlayerState);
	void Broadcast(PlayerStatePtr inPlayerState);

	void Test(PlayerStatePtr inPlayerState, const int32 inIValue, const std::string inSValue)
	{
		printf("%d %s\n", inIValue, inSValue.c_str());
	}

private:
	std::map<uint64, PlayerStatePtr> mPlayers;
};

