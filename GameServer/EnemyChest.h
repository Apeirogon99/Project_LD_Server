#pragma once
class EnemyChest : public EnemyCharacter
{
public:
	EnemyChest();
	virtual ~EnemyChest();

public:
	virtual void OnInitialization()	override;

	virtual void OnReward() override;
};

