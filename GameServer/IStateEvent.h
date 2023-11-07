#pragma once

enum class EStateType : uint8
{
	State_Unspecified,		//존재하지 않는 상태
	State_Idle,				//가만히 있는 상태
	State_Round,			//주변을 이동함
	State_Recovery,			//복귀 상태
	State_Search,			//서치 상태
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
	float		mMaxDistanceLength;
	Location	mCurrentLocation;
};

class SearchState : public IStateEvent
{
public:
	SearchState() : IStateEvent(EStateType::State_Search) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;

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

class StunState : public IStateEvent
{
public:
	StunState() : IStateEvent(EStateType::State_Stun), mStunTime(0), mStunToChaseTime(0) { }

public:
	virtual void Enter(EnemyCharacterRef inEnemy)							override;
	virtual void Update(EnemyCharacterRef inEnemy, const int64 inDeltaTime)	override;
	virtual void Exit(EnemyCharacterRef inEnemy)							override;

public:
	void SetStunTime(const int64 inStunTime);

private:
	int64 mStunTime;
	int64 mStunToChaseTime;
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
	StateManager();
	~StateManager();

	StateManager(const StateManager&) = delete;
	StateManager(StateManager&&) noexcept = delete;

	StateManager& operator=(const StateManager&) = delete;
	StateManager& operator=(StateManager&&) noexcept = delete;

public:
	void SetEnemy(EnemyCharacterRef inEnemy);
	void SetState(const EStateType& inStateType);

public:
	const EStateType& GetCurrentStateType() const;

public:
	void EnterState()
	{
		IStateEvent* curState = mStateTypes.at(mCurrentState);
		curState->Enter(mEnemy);
	}

	void UpdateState(const int64 inDeltaTime)
	{
		if (mCurrentState != EStateType::State_Unspecified)
		{
			IStateEvent* curState = mStateTypes.at(mCurrentState);
			curState->Update(mEnemy, inDeltaTime);
		}
	}

	void ExitState()
	{
		IStateEvent* curState = mStateTypes.at(mCurrentState);
		curState->Exit(mEnemy);
	}

public:
	void StateChangeDebugPrint();
	const std::wstring ToStringState(const EStateType& type);
	IStateEvent* GetCurrentStateEvent();

private:
	EnemyCharacterRef					mEnemy;
	std::map<EStateType, IStateEvent*>	mStateTypes;
	EStateType							mOldState;
	EStateType							mCurrentState;
};