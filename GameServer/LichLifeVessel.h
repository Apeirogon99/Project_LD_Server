#pragma once
class LichLifeVessel : public EnemyCharacter
{
public:
	LichLifeVessel();
	virtual ~LichLifeVessel();

public:
	virtual void OnInitialization()					override;

	virtual void OnReward() override;
};

