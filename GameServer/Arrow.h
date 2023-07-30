#pragma once
class Arrow : public Actor
{
public:
	Arrow();
	~Arrow();

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
	void SetDamage();
	
private:
	float mDamage;

	ProjectileComponent			mProjectileComponent;
	SphereCollisionComponent	mCollisionComponent;
};

