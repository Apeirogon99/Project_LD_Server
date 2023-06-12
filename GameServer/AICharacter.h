#pragma once
class AICharacter : public Pawn
{
public:
	AICharacter(const WCHAR* inName);
	virtual ~AICharacter();

public:
	virtual void Initialization()	override {}
	virtual void Destroy()			override {}
	virtual void Tick()				override {}
	virtual bool IsValid()			override { return true; }

public:
	virtual void AppearActor(PlayerStatePtr inClosePlayerState) abstract {}
	virtual void DisAppearActor(PlayerStatePtr inClosePlayerState) abstract {}

};

