#pragma once
class EnemySlime : public EnemyCharacter
{
public:
	EnemySlime();
	virtual ~EnemySlime();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) override;
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) override;

public:
	virtual void OnMovement() override {}

protected:

};

