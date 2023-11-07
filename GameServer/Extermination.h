#pragma once
class Extermination : public EnemyAttack
{
public:
	Extermination();
	virtual ~Extermination();

	Extermination(const Extermination&) = delete;
	Extermination(Extermination&&) noexcept = delete;

	Extermination& operator=(const Extermination&) = delete;
	Extermination& operator=(Extermination&&) noexcept = delete;

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
	virtual bool OnParrying(ActorPtr inActor) override;

public:
	SphereCollisionComponent* GetSphereCollisionComponent();

private:
	float mStartSphereSize;
};

