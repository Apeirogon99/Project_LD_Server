#pragma once

enum class EEnemyAttackType
{
	Enemy_Attack_Unspecified,
	Enemy_Attack_Melee,
	Enemy_Attack_Projectile,
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
	void ReserveDestroy(const int64& inDelay);
	void SetEnemyAttackType(const EEnemyAttackType& inEnemyAttackType);
	void SetTargetActorType(const EActorType& inTargetActorType);
	void SetDamage(const float& inDamage);

public:
	const EEnemyAttackType&		GetEnemyAttackType() const;
	const EActorType&			GetTargetActorType() const;
	const float&				GetDamage() const;

protected:
	EEnemyAttackType	mEnemyAttackType;
	EActorType			mTargetActorType;

	float				mDamage;
};

