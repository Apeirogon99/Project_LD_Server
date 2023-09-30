#pragma once

class NPCQuest
{
public:
	NPCQuest();
	~NPCQuest();

public:

private:
	int32 mQuestID;
	int32 mPreQuestID;
};

class NPCharacter : public Character
{
public:
	NPCharacter(const WCHAR* inName);
	virtual ~NPCharacter();

public:
	virtual void OnInitialization()								override;
	virtual void OnDestroy()									override;
	virtual void OnTick(const int64 inDeltaTime)				override;
	virtual bool IsValid()										override;

public:
	virtual void OnAppearActor(ActorPtr inAppearActor)			override;
	virtual void OnDisAppearActor(ActorPtr inDisappearActor)	override;

public:
	void RequestQuest(PlayerStatePtr inPlayerState, int32 inQuestID);


private:
	int32 mNPCID;
	std::vector<NPCQuest> mQuests;
};

