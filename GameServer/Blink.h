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
	void TeleportPlayerLocation(const FVector inLocation, const FRotator inRotation);
	void TeleportSafeLocation(const FVector inLocation, const FRotator inRotation);

public:
	BoxCollisionComponent* GetBoxCollisionComponent() const;

protected:
	int64 mStartTime;
	int32 mBlinkID;
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
	virtual void OnParrying(ActorPtr inActor) override;

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
	virtual void OnParrying(ActorPtr inActor) override;

public:
	void SturnBeign(PlayerCharacterPtr inPlayer, float inMovementSpeed);
	void SturnEnd(PlayerCharacterPtr inPlayer, float inMovementSpeed);
};