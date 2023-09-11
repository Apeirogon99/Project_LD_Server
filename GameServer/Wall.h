#pragma once
class Wall : public Actor
{
public:
	Wall();
	virtual ~Wall();

private:
	Wall(Wall&&) = delete;
	Wall& operator=(Wall&&) = delete;

public:
	virtual void OnInitialization()				 override;
	virtual void OnDestroy()					 override;
	virtual void OnTick(const int64 inDeltaTime) override;
	virtual bool IsValid()						 override;

public:
	void SetBoxCollisionExtent(FVector inExtent);

public:
	BoxCollisionComponent* GetBoxCollisionComponent() const;
};
