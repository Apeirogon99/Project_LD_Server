#pragma once
class Wall : public Obstruction
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

};
