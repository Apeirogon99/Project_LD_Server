#pragma once
class OnslaughtOfShadows : public EnemyAttack
{
public:
	OnslaughtOfShadows();
	virtual ~OnslaughtOfShadows();

	OnslaughtOfShadows(const OnslaughtOfShadows&) = delete;
	OnslaughtOfShadows(OnslaughtOfShadows&&) noexcept = delete;

	OnslaughtOfShadows& operator=(const OnslaughtOfShadows&) = delete;
	OnslaughtOfShadows& operator=(OnslaughtOfShadows&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	virtual void CheackCollision() override;
	virtual void OnParrying(ActorPtr inActor) override;

public:
	BoxCollisionComponent* GetBoxCollisionComponent();

};

