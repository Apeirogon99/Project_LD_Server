#pragma once
class EnemySlime : public EnemyCharacter
{
public:
	EnemySlime();
	virtual ~EnemySlime();

public:
	virtual void Initialization()	override;
	virtual void Destroy()			override;
	virtual void Tick()				override;
	virtual bool IsValid()			override;

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) override;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) override;

protected:
	virtual void Attack() override;
	virtual void Hit() override;
	virtual void Death() override;

};

