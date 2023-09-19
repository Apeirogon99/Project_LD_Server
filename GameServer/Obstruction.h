#pragma once

class Obstruction : public Actor
{
public:
	Obstruction();
	virtual ~Obstruction();

private:
	Obstruction(Obstruction&&) = delete;
	Obstruction& operator=(Obstruction&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid()						 override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor) override;
	virtual void OnDisAppearActor(ActorPtr inDisAppearActor) override;

public:
	void SetObstructionName(const WCHAR* inName);
	void SetObstructionType(const int32& inType);
	void SetBoxCollisionExtent(FVector inExtent);

public:
	BoxCollisionComponent* GetBoxCollisionComponent() const;

protected:
	int32 mObstructionType;
};

