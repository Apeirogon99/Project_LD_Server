#pragma once

enum class EStateEventType : uint8
{
	State_None,
	State_Idle,
	State_Walk,
	State_Chase,
	State_Attack,
};

class IStateEvent
{
public:
	IStateEvent(EStateEventType inType) : mStateEventType(inType) { }

public:
	virtual void Enter()		abstract;
	virtual void FixedUpdate()	abstract;
	virtual void Update()		abstract;
	virtual void Exit()			abstract;

public:
	const EStateEventType& GetStateEventType() const { return mStateEventType; }

private:
	EStateEventType mStateEventType;
};

class StateManager
{
public:
	StateManager() : mStateEvent(nullptr) {}
	~StateManager() {}

	StateManager(const StateManager&) = delete;
	StateManager(StateManager&&) noexcept = delete;

	StateManager& operator=(const StateManager&) = delete;
	StateManager& operator=(StateManager&&) noexcept = delete;

public:
	void SetState(const EStateEventType& inStateType)
	{

		if (nullptr == mStateEvent)
		{
			//mStateEvent = new IStateEvent(inStateType);
			return;
		}

		if (mStateEvent->GetStateEventType() == inStateType)
		{
			return;
		}

		mStateEvent->Exit();

		delete mStateEvent;
		mStateEvent = nullptr;

		//mStateEvent = new IStateEvent(inStateType);

		mStateEvent->Enter();
	}

	void EnterState()
	{
		mStateEvent->Enter();
	}

	void UpdateState()
	{
		mStateEvent->Update();
	}

	void FixedUpdateState()
	{
		mStateEvent->FixedUpdate();
	}

	void ReleaseState()
	{
		mStateEvent->Exit();
	}

private:
	IStateEvent* mStateEvent;
};

class IdleState : public IStateEvent
{
public:
	IdleState() : IStateEvent(EStateEventType::State_Idle) { }

public:
	virtual void Enter()		override;
	virtual void FixedUpdate()	override;
	virtual void Update()		override;
	virtual void Exit()			override;
};

class ChaseState : public IStateEvent
{
public:
	ChaseState() : IStateEvent(EStateEventType::State_Chase) { }

public:
	virtual void Enter()		override;
	virtual void FixedUpdate()	override;
	virtual void Update()		override;
	virtual void Exit()			override;
};

class AttackState : public IStateEvent
{
public:
	AttackState() : IStateEvent(EStateEventType::State_Attack) { }

public:
	virtual void Enter()		override;
	virtual void FixedUpdate()	override;
	virtual void Update()		override;
	virtual void Exit()			override;
};