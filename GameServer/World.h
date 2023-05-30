#pragma once

class World : public GameObject
{
public:
	World(const SessionManagerRef& inSessionManager, const WCHAR* inName);
	virtual ~World();

protected:
	virtual void Initialization() override;
	virtual void Destroy() override;
	virtual bool IsValid() override;

public:
	virtual void Tick() override;

public:
	void Enter(PlayerStatePtr inPlayerState);
	void Leave(PlayerStatePtr inPlayerState);
	void Broadcast(SendBufferPtr sendBuffer);

	void AppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState);
	void DisAppearCharacter(PlayerStatePtr inTargetPlayerState, PlayerStatePtr inAppearPlayerState);
	void MoveDestination(PlayerStatePtr inPlayerState, Protocol::C2S_MovementCharacter inPakcet);

	void InsertItemToInventory(PlayerStatePtr inPlayerState, Protocol::C2S_InsertInventory inPacket);
	void LoadItemToInventory(PlayerStatePtr inPlayerState, Protocol::C2S_LoadInventory inPacket);
	void UpdateItemToInventory(PlayerStatePtr inPlayerState, Protocol::C2S_UpdateInventory inPacket);
	void DeleteItemToInventory(PlayerStatePtr inPlayerState, Protocol::C2S_DeleteInventory inPacket);

	template<typename T>
	ActorPtr CreateActor(const Protocol::SVector& inLocation, const Protocol::SRotator& inRotator);
	bool InsertActor(const int64 inGameObjectID, ActorPtr inActor);
	bool DestroyActor(const int64 inGameObjectID);
	
	//TEMP:
	void CreateTempItem()
	{
		int32 tempArray[] = { 1, 22, 43, 62, 81, 102, 122 };
		for (int32 i = 0; i < 7; ++i)
		{
			Protocol::SItem item;
			item.set_item_code(tempArray[i]);

			Protocol::SVector* location = item.mutable_world_position();
			location->set_x(0);
			location->set_y((i + 1) * 150);
			location->set_z(50);

			Protocol::SRotator rotation;
			rotation.set_pitch(0);
			rotation.set_roll(0);
			rotation.set_yaw(0);

			AItemPtr newItem = std::static_pointer_cast<AItem>(CreateActor<AItem>(*location, rotation));
			newItem->Init(item);

		}
	}

public:
	WorldRef	GetWorldRef();
	const int64 GetNextGameObjectID();

protected:
	bool IsValidPlayer(RemotePlayerPtr inRemotePlayer);
	bool IsValidGameObject(ActorPtr inActor);

private:
	std::map<int64, PlayerStatePtr> mPlayers;
	std::map<int64, ActorPtr> mActors;
	int64 mPlayersCount;
	int64 mGameObjectID;
};

template<typename T>
inline ActorPtr World::CreateActor(const Protocol::SVector& inLocation, const Protocol::SRotator& inRotator)
{
	const int64 gameObject = GetNextGameObjectID();
	ActorPtr object = std::static_pointer_cast<Actor>(std::make_shared<T>(gameObject));
	object->Initialization();
	object->SetLocation(inLocation);
	object->SetRotator(inRotator);

	bool result = InsertActor(gameObject, object);

	return object;
}
