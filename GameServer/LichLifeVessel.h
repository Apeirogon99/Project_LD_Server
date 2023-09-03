#pragma once
class LichLifeVessel : public EnemyCharacter
{
public:
	LichLifeVessel();
	virtual ~LichLifeVessel();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

	virtual void OnReward() override;
};

