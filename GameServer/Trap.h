#pragma once

template<typename T>
class Trap : public BoxTrigger
{
public:
	Trap();
	virtual ~Trap();

private:
	Trap(Trap&&) = delete;
	Trap& operator=(Trap&&) = delete;

public:
	virtual void OnInitialization() override;

public:
	virtual void OnBeginOverlap(ActorPtr inBeginOverlapActor) override;
	virtual void OnEndOverlap(ActorPtr inEndOverlapActor) override;
	virtual void OnOverlapTick(const int64 inDeltaTime) override;

public:
	void EnterTrap();
	void EndTrap();
	void SetEnterTrapCallBackFunction(std::function<void(T&)> inTrapCallBackFunction);
	void SetEndTrapCallBackFunction(std::function<void(T&)> inTrapCallBackFunction);

private:
	bool mIsTrap;
	std::function<void(T&)> mEnterCallBack;
	std::function<void(T&)> mEndCallBack;
};

template<typename T>
inline Trap<T>::Trap() : mIsTrap(false)
{
}

template<typename T>
inline Trap<T>::~Trap()
{
}

template<typename T>
inline void Trap<T>::OnInitialization()
{
	this->SetTick(true, GAME_TICK);

	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(50.0f, 50.0f, 50.0f));
}

template<typename T>
inline void Trap<T>::OnBeginOverlap(ActorPtr inBeginOverlapActor)
{
	if (mMaxOverlap == mOverlapActor.size() && false == mIsTrap)
	{
		EnterTrap();
	}
}

template<typename T>
inline void Trap<T>::OnEndOverlap(ActorPtr inEndOverlapActor)
{
	if (true == mIsTrap)
	{
		EndTrap();
	}
}

template<typename T>
inline void Trap<T>::OnOverlapTick(const int64 inDeltaTime)
{
}

template<typename T>
inline void Trap<T>::EnterTrap()
{

	if (!mEnterCallBack)
	{
		return;
	}

	std::shared_ptr<T> owner = std::static_pointer_cast<T>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	T* base = owner.get();
	if (nullptr == base)
	{
		return;
	}

	mIsTrap = true;

	mEnterCallBack(*base);
}

template<typename T>
inline void Trap<T>::EndTrap()
{

	if (!mEndCallBack)
	{
		return;
	}

	std::shared_ptr<T> owner = std::static_pointer_cast<T>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	T* base = owner.get();
	if (nullptr == base)
	{
		return;
	}

	mIsTrap = false;

	mEndCallBack(*base);
}

template<typename T>
inline void Trap<T>::SetEnterTrapCallBackFunction(std::function<void(T&)> inTrapCallBackFunction)
{
	mEnterCallBack = inTrapCallBackFunction;
}

template<typename T>
inline void Trap<T>::SetEndTrapCallBackFunction(std::function<void(T&)> inTrapCallBackFunction)
{
	mEndCallBack = inTrapCallBackFunction;
}
