#pragma once

class World : public GameObject
{
public:
	World(const WCHAR* inName, GameTaskPtr inGameTask);
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

	void VisibleAreaSync();

	template<typename T>
	ActorPtr CreateActor(const Protocol::SVector& inLocation, const Protocol::SRotator& inRotator);
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
	GameTaskPtr	GetGameTask() { return mGameTask; }
	WorldRef	GetWorldRef();

private:
	GameTaskPtr							mGameTask;
	std::map<int64, PlayerStatePtr>		mPlayerStates;
	std::map<int64, ActorPtr>			mWorldActors;
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

	mGameTask->PushTask(object->GetGameObjectPtr());

	object->SetLocation(inLocation);
	object->SetRotation(inRotator);

	std::pair<int64, ActorPtr> newObject = std::make_pair(object->GetGameObjectID(), object);
	auto result = mWorldActors.insert(newObject);
	return object;
}
