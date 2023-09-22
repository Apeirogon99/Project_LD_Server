#pragma once
class Trigger : public Actor
{
public:
	Trigger(const WCHAR* inName);
	virtual ~Trigger();

private:
	Trigger(Trigger&&) = delete;
	Trigger& operator=(Trigger&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual bool IsValid()						 override;

public:
	virtual void OnOverlapTick(const int64 inDeltaTime) {}

public:
	void SetOverlapType(EActorType inOverlapActorType);
	void SetTrigger(bool inIsTrigger);
	void SetMaxOverlap(uint32 inMaxOverlap = INFINITE);
	virtual void OnBeginOverlap(ActorPtr inBeginOverlapActor) {}
	virtual void OnEndOverlap(ActorPtr inEndOverlapActor) {}

protected:
	EActorType					mOverlapType;
	bool						mIsTrigger;
	uint32						mMaxOverlap;
	std::map<ActorPtr, bool>	mOverlapActor;
};

class BoxTrigger : public Trigger
{
public:
	BoxTrigger();
	virtual ~BoxTrigger();

public:
	virtual void OnTick(const int64 inDeltaTime) override;

public:
	void SetBoxTriggerExtent(FVector inExtent);

public:
	BoxCollisionComponent* GetBoxCollisionComponent() const;

private:
	BoxTrigger(BoxTrigger&&) = delete;
	BoxTrigger& operator=(BoxTrigger&&) = delete;

};

class SphereTrigger : public Trigger
{
public:
	SphereTrigger();
	virtual ~SphereTrigger();

public:
	virtual void OnTick(const int64 inDeltaTime) override;

public:
	void SetSphereTriggerRadius(float inRadius);

public:
	SphereCollisionComponent* GetSphereCollisionComponent() const;

private:
	SphereTrigger(SphereTrigger&&) = delete;
	SphereTrigger& operator=(SphereTrigger&&) = delete;

};