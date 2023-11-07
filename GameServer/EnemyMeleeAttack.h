#pragma once
class EnemyMeleeAttack : public EnemyAttack
{
public:
	EnemyMeleeAttack();
	virtual ~EnemyMeleeAttack();

	EnemyMeleeAttack(const EnemyMeleeAttack&) = delete;
	EnemyMeleeAttack(EnemyMeleeAttack&&) noexcept = delete;

	EnemyMeleeAttack& operator=(const EnemyMeleeAttack&) = delete;
	EnemyMeleeAttack& operator=(EnemyMeleeAttack&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override {}
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override {}

public:
	virtual void CheackCollision() override;
	virtual bool OnParrying(ActorPtr inActor) override;

public:
	void SetAttackExtent(const FVector inExtent);

public:
	BoxCollisionComponent* GetBoxCollisionComponent() const;

public:
	bool mIsParrying;
};

