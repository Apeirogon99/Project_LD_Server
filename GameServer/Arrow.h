#pragma once
class Arrow : public Actor
{
public:
	Arrow();
	virtual ~Arrow();

	Arrow(const Arrow&) = delete;
	Arrow(Arrow&&) noexcept = delete;

	Arrow& operator=(const Arrow&) = delete;
	Arrow& operator=(Arrow&&) noexcept = delete;

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor) override;

public:
	void SyncLocation(const int64 inDeltaTime);
	void CheackCollision();

public:
	void SetDamage(const float& inDamage);
	void SetLifeTime(const int64& inLifeTime);

public:
	const float					GetDamage() const;
	const int64					GetLifeTime() const;
	ProjectileComponent&		GetProjectileComponent();
	SphereCollisionComponent&	GetSphereCollisionComponent();
	
private:
	bool mIsCollision;
	float mDamage;
	int64 mLifeTime;
	
	ProjectileComponent			mProjectileComponent;
	SphereCollisionComponent	mCollisionComponent;
};

