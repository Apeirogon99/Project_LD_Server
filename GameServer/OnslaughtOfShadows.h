#pragma once

class Plane
{
public:
	Plane() : mP1(), mP2() {}
	Plane(FVector inP1, FVector inP2) : mP1(inP1), mP2(inP2) {}

	FVector mP1;
	FVector mP2;
};

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
	void SetOnslaughtOfShadows(FVector inLU, FVector inRU, FVector inRD, FVector inLD, FVector inOriginLocation);
	
	virtual void CheackCollision() override;
	virtual void OnParrying(ActorPtr inActor) override;

protected:
	bool GetIntersection(FVector inPointA1, FVector inPointA2, FVector inPointB1, FVector inPointB2, FVector& outIntersection);

public:
	BoxCollisionComponent* GetBoxCollisionComponent();
	
private:
	bool mActive;
	FVector mOriginLocation;
	std::set<int64>	mOverlapGameObjectID;
};

