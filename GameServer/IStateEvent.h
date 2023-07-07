#pragma once

enum class EStateType : uint8
{
	State_Unspecified,		//존재하지 않는 상태
	State_Idle,				//가만히 있는 상태
	State_Round,			//주변을 이동함
	State_Recovery,			//복귀 상태
	State_Chase,			//추적 상태
	State_Attack,			//공격 상태
	State_Hit,				//맞은 상태
	State_Stun,				//스턴 상태
	State_Death				//죽음 상태
};

class IdleState;
class RoundState;
class RecoveryState;
class ChaseState;
class AttackState;
class HitState;
class DeathState;

class IStateEvent
{
public:
	IStateEvent(EStateType inType) : mStateEventType(inType) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							abstract;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	abstract;
	virtual void Exit(EnemyCharacterRef inEnemy)							abstract;

public:
	const EStateType& GetStateEventType() const { return mStateEventType; }

private:
	EStateType mStateEventType;
};

class IdleState : public IStateEvent
{
public:
	IdleState() : IStateEvent(EStateType::State_Idle), mIdleToRoundTime(0) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;

private:
	int64 mIdleToRoundTime;
};

class RoundState : public IStateEvent
{
public:
	RoundState() : IStateEvent(EStateType::State_Round) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;
};

class RecoveryState : public IStateEvent
{
public:
	RecoveryState() : IStateEvent(EStateType::State_Recovery), mMaxDistanceLength(0.0f) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;

private:
	float mMaxDistanceLength;
};

class ChaseState : public IStateEvent
{
public:
	ChaseState() : IStateEvent(EStateType::State_Chase), mChaseToRecoveryTime(0){ }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;

private:
	int64 mChaseToRecoveryTime;
};

class AttackState : public IStateEvent
{
public:
	AttackState() : IStateEvent(EStateType::State_Attack) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;
};

class HitState : public IStateEvent
{
public:
	HitState() : IStateEvent(EStateType::State_Hit), mHitToChaseTime(0) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;

private:
	int64 mHitToChaseTime;
};

class DeathState : public IStateEvent
{
public:
	DeathState() : IStateEvent(EStateType::State_Death) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;
};

class StateManager
{
public:
	StateManager() : mCurrentState(EStateType::State_Unspecified)
	{
		mStateTypes.insert(std::make_pair(EStateType::State_Idle,		static_cast<IStateEvent*>(new IdleState())));
		mStateTypes.insert(std::make_pair(EStateType::State_Round,		static_cast<IStateEvent*>(new RoundState())));
		mStateTypes.insert(std::make_pair(EStateType::State_Recovery,	static_cast<IStateEvent*>(new RecoveryState())));
		mStateTypes.insert(std::make_pair(EStateType::State_Chase,		static_cast<IStateEvent*>(new ChaseState())));
		mStateTypes.insert(std::make_pair(EStateType::State_Attack,		static_cast<IStateEvent*>(new AttackState())));
		mStateTypes.insert(std::make_pair(EStateType::State_Hit,		static_cast<IStateEvent*>(new HitState())));
		mStateTypes.insert(std::make_pair(EStateType::State_Death,		static_cast<IStateEvent*>(new DeathState())));
	}
	~StateManager()
	{
		for (std::map<EStateType, IStateEvent*>::iterator state = mStateTypes.begin(); state != mStateTypes.end(); state++)
		{
			delete state->second;
		}
	}

	StateManager(const StateManager&) = delete;
	StateManager(StateManager&&) noexcept = delete;

	StateManager& operator=(const StateManager&) = delete;
	StateManager& operator=(StateManager&&) noexcept = delete;

public:
	void SetEnemy(EnemyCharacterRef inEnemy)
	{
		mEnemy = inEnemy;
	}

	void SetState(const EStateType& inStateType)
	{

		if (mCurrentState == EStateType::State_Unspecified)
		{
			mCurrentState = inStateType;
			EnterState();
			return;
		}

		if (mCurrentState == inStateType)
		{
			return;
		}

		StateChangeDebugPrint(mCurrentState, inStateType);

		IStateEvent* oldState = mStateTypes.at(mCurrentState);
		oldState->Exit(mEnemy);

		mCurrentState = inStateType;

		IStateEvent* newState = mStateTypes.at(mCurrentState);
		newState->Enter(mEnemy);
	}

public:
	const EStateType& GetCurrentStateType() const
	{
		return mCurrentState;
	}

public:
	void EnterState()
	{
		IStateEvent* curState = mStateTypes.at(mCurrentState);
		curState->Enter(mEnemy);
	}

	void UpdateState(const int64 inDeltaTime)
	{
		IStateEvent* curState = mStateTypes.at(mCurrentState);
		curState->Update(mEnemy, inDeltaTime);
	}

	void ExitState()
	{
		IStateEvent* curState = mStateTypes.at(mCurrentState);
		curState->Exit(mEnemy);
	}

protected:
	void StateChangeDebugPrint(const EStateType& inOldType, const EStateType& inNewType);
	const std::wstring ToStringState(const EStateType& type)
	{
		std::wstring stateStr;
		switch (type)
		{
		case EStateType::State_Unspecified:
			stateStr = L"Unspecified";
			break;
		case EStateType::State_Idle:
			stateStr = L"Idle";
			break;
		case EStateType::State_Round:
			stateStr = L"Round";
			break;
		case EStateType::State_Recovery:
			stateStr = L"Recovery";
			break;
		case EStateType::State_Chase:
			stateStr = L"Chase";
			break;
		case EStateType::State_Attack:
			stateStr = L"Attack";
			break;
		case EStateType::State_Hit:
			stateStr = L"Hit";
			break;
		case EStateType::State_Stun:
			stateStr = L"Stun";
			break;
		case EStateType::State_Death:
			stateStr = L"Death";
			break;
		default:
			break;
		}
		return stateStr;
	}

private:
	EnemyCharacterRef					mEnemy;
	std::map<EStateType, IStateEvent*>	mStateTypes;
	EStateType							mCurrentState;
};