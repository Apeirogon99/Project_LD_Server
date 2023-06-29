#pragma once

class World : public GameObject
{
public:
	World(const WCHAR* inName, GameTaskPtr inGameTask);
	virtual ~World();

protected:
	virtual void OnInitialization() override;
	virtual void OnDestroy() override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid() override;

public:
	void ServerTravel(PlayerStatePtr inPlayerState, Protocol::C2S_TravelServer inPacket);
	void Enter(PlayerStatePtr inPlayerState, Protocol::C2S_EnterGameServer inPacket);
	void Leave(PlayerStatePtr inPlayerState);

	void VisibleAreaInit(PlayerStatePtr inPlayerState);
	void VisibleAreaSync();
	void CheackToken();

	template<typename T>
	ActorPtr CreateActor(const Protocol::SVector& inLocation, const Protocol::SRotator& inRotator);
	bool DestroyActor(const int64 inGameObjectID);
	bool IsValidActor(const int64 inGameObjectID);
	
public:
	GameTaskPtr	GetGameTask() { return mGameTask; }
	WorldRef	GetWorldRef() { return std::static_pointer_cast<World>(shared_from_this()); }
	GameDatasPtr GetGameDatas() { return mGameTask->GetGameDatas(); }
	const int64 GetServiceTimeStamp() { return mGameTask->GetService()->GetServiceTimeStamp(); }

private:
	GameTaskPtr							mGameTask;
	std::map<int64, PlayerStatePtr>		mPlayerStates;
	std::map<int64, ActorPtr>			mWorldActors;

	std::vector<class Token>			mTokens;
};

template<typename T>
inline ActorPtr World::CreateActor(const Protocol::SVector& inLocation, const Protocol::SRotator& inRotator)
{
	if (nullptr == mGameTask)
	{
		return nullptr;
	}

	ActorPtr object = std::static_pointer_cast<Actor>(std::make_shared<T>());
	if (nullptr == object)
	{
		return nullptr;
	}
	GameObjectPtr gameObject = object->GetGameObjectPtr();
	mGameTask->PushTask(gameObject);

	object->SetWorld(GetWorldRef());
	object->SetLocation(inLocation);
	object->SetRotation(inRotator);

	const int64 objectID = object->GetGameObjectID();
	std::pair<int64, ActorPtr> newObject = std::make_pair(objectID, object);
	auto result = mWorldActors.insert(newObject);
	return object;
}
