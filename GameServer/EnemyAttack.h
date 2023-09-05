#pragma once

enum class EEnemyAttackType
{
	Enemy_Attack_Unspecified,
	Enemy_Attack_Nomal_Melee,
	Enemy_Attack_Hard_Melee,
	Enemy_Attack_Nomal_Projectile,
	Enemy_Attack_Hard_Projectile,
	Enemy_Attack_Nomal_Place,
	Enemy_Attack_Hard_Place,
};

class EnemyAttack : public Actor
{
public:
	EnemyAttack(const WCHAR* inName);
	virtual ~EnemyAttack();

	EnemyAttack(const EnemyAttack&) = delete;
	EnemyAttack(EnemyAttack&&) noexcept = delete;

	EnemyAttack& operator=(const EnemyAttack&) = delete;
	EnemyAttack& operator=(EnemyAttack&&) noexcept = delete;

public:
	virtual void OnInitialization()					abstract;
	virtual void OnDestroy()						abstract;
	virtual void OnTick(const int64 inDeltaTime)	abstract;
	virtual bool IsValid()							abstract;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) abstract;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) abstract;

public:
	virtual void CheackCollision() abstract;
	virtual void OnParrying(ActorPtr inActor) abstract;
	virtual void OnMovement() {}

public:
	void ReserveDestroy(const int64& inDelay);
	void SetEnemyAttackType(const EEnemyAttackType& inEnemyAttackType);
	void SetTargetActorType(const EActorType& inTargetActorType);
	void SetDamage(const float& inDamage);
	void SetCancel(const bool& inCancel);
	void SetParryinglTime(const int64& inStart, const int64& inEnd);

public:
	const EEnemyAttackType&		GetEnemyAttackType() const;
	const EActorType&			GetTargetActorType() const;
	const float&				GetDamage() const;
	const bool&					GetCancel() const;
	const int64&				GetMaxLifeTime() const;
	const bool					CanParrying() const;
	const bool					IsLife(const int64& inDeltaTime);

protected:
	EEnemyAttackType	mEnemyAttackType;
	EActorType			mTargetActorType;

	float				mDamage;
	bool				mCancel;

	bool				mIsLife;
	int64				mMaxLifeTime;
	int64				mCurrentLifeTime;

	int64				mParryingStart;
	int64				mParryingEnd;
};

