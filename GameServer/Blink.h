#pragma once
class Blink : public EnemyAttack
{
public:
	Blink(const WCHAR* inName);
	virtual ~Blink();

	Blink(const Blink&) = delete;
	Blink(Blink&&) noexcept = delete;

	Blink& operator=(const Blink&) = delete;
	Blink& operator=(Blink&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	void SetBlinkID(const ESkillID& inSkillID);
	void ReActionSkill(const FVector inLocation, const FRotator inRotator);
	void TeleportPlayerLocation(const FVector inLocation, const FRotator inRotation);
	void TeleportSafeLocation(const FVector inLocation, const FRotator inRotation);

public:
	BoxCollisionComponent* GetBoxCollisionComponent() const;

public:
	int64 mStartTime;
	int32 mBlinkID;

	int64		mSafeTime;
	FVector		mSafeLocation;
	FRotator	mSafeRotation;

	int64		mDestroyTime;
};

class BlinkAttack : public Blink
{
public:
	BlinkAttack();
	virtual ~BlinkAttack();

	BlinkAttack(const BlinkAttack&) = delete;
	BlinkAttack(BlinkAttack&&) noexcept = delete;

	BlinkAttack& operator=(const BlinkAttack&) = delete;
	BlinkAttack& operator=(BlinkAttack&&) noexcept = delete;

public:
	virtual void CheackCollision() override;
	virtual bool OnParrying(ActorPtr inActor) override;

};

class BlinkSturn : public Blink
{
public:
	BlinkSturn();
	virtual ~BlinkSturn();

	BlinkSturn(const BlinkSturn&) = delete;
	BlinkSturn(BlinkSturn&&) noexcept = delete;

	BlinkSturn& operator=(const BlinkSturn&) = delete;
	BlinkSturn& operator=(BlinkSturn&&) noexcept = delete;

public:
	virtual void CheackCollision() override;
	virtual bool OnParrying(ActorPtr inActor) override;

public:
	void SturnBeign(PlayerCharacterPtr inPlayer, float inMovementSpeed);
	void SturnEnd(PlayerCharacterPtr inPlayer, float inMovementSpeed);
};