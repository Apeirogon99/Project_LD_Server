#pragma once
class Pawn : public Actor
{
public:
	Pawn(const WCHAR* inName);
	virtual ~Pawn();

public:
	virtual void Initialization()	override {}
	virtual void Destroy()			override {}
	virtual void Tick()				override {}
	virtual bool IsValid()			override { return true; }

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) override {}
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) override {}
};

