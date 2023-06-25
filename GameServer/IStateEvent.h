#pragma once

enum class EStateEventType : uint8
{
	None,
	Idle,
	Walk,
	Chase,
	Attack,
};

class IStateEvent
{
public:
	IStateEvent(EStateEventType inType);

public:
	virtual void Enter() {}
	virtual void FixedUpdate() {}
	virtual void Update() {}
	virtual void Exit() {}

public:
	const EStateEventType& GetStateEventType() { return mStateEventType; }

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
			mStateEvent = new IStateEvent(inStateType);
			return;
		}

		if (mStateEvent->GetStateEventType() == inStateType)
		{
			return;
		}

		mStateEvent->Exit();

		delete mStateEvent;
		mStateEvent = nullptr;

		mStateEvent = new IStateEvent(inStateType);

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
	IdleState() : IStateEvent(EStateEventType::Idle) { }
};

class WalkState : public IStateEvent
{
public:
	WalkState() : IStateEvent(EStateEventType::Walk) { }
};

class ChaseState : public IStateEvent
{
public:
	ChaseState() : IStateEvent(EStateEventType::Chase) { }
};

class AttackState : public IStateEvent
{
public:
	AttackState() : IStateEvent(EStateEventType::Attack) { }
};