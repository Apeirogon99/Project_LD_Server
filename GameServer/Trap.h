#pragma once

template<typename T, typename... Args>
class Trap : public BoxTrigger
{
public:
	Trap() : mIsTrap(false), BoxTrigger()
	{

	}

	virtual ~Trap()
	{

	}

private:
	Trap(Trap&&) = delete;
	Trap& operator=(Trap&&) = delete;

public:
	virtual void OnInitialization() override
	{
		this->SetTick(true, GAME_TICK);

		BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
		collision->SetOwner(this->GetActorRef());
		collision->SetBoxCollision(FVector(50.0f, 50.0f, 50.0f));
	}

public:
	virtual void OnBeginOverlap(ActorPtr inBeginOverlapActor) override
	{
		if (mMaxOverlap == mOverlapActor.size() && false == mIsTrap)
		{
			EnterTrap();
		}
	}

	virtual void OnEndOverlap(ActorPtr inEndOverlapActor) override
	{
		if (true == mIsTrap)
		{
			EndTrap();
		}
	}

	virtual void OnOverlapTick(const int64 inDeltaTime) override
	{

	}

public:
	void EnterTrap()
	{
		if (!mEnterCallBack)
		{
			return;
		}

		mIsTrap = true;
		mEnterCallBack();
	}

	void EndTrap()
	{
		if (!mEndCallBack)
		{
			return;
		}

		mIsTrap = false;
		mEndCallBack();
	}

	void SetEnterTrapCallBackFunction(std::function<void(T&, Args...)> inTrapCallBackFunction, Args... inArgs)
	{
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

		mEnterCallBack = std::bind(inTrapCallBackFunction, std::ref(*base), inArgs...);
	}

	void SetEndTrapCallBackFunction(std::function<void(T&, Args...)> inTrapCallBackFunction, Args... inArgs)
	{
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

		mEnterCallBack = std::bind(inTrapCallBackFunction, std::ref(*base), inArgs...);
	}

private:
	bool mIsTrap;
	std::function<void()> mEnterCallBack;
	std::function<void()> mEndCallBack;
};
