#pragma once
class CharacterManager : public GameObject
{
public:
	CharacterManager();
	virtual ~CharacterManager();

public:
	virtual void OnInitialization()					override;
	virtual void OnDestroy()						override;
	virtual void OnTick(const int64 inDeltaTime)	override;
	virtual bool IsValid()							override;

public:
	void LoadCharacter();
	void CreateCharacter(Protocol::C2S_CreateCharacter inPacket);
	void DeleteCharacter(Protocol::C2S_DeleteCharacter inPacket);

	void PushCharacter(const int32 inCharacterID, const Protocol::SCharacterData* inCharacterData);
	void ReleaseCharacter(const int32 inCharacterID);
	void ClearCharacter();

public:
	LoginCharacterPtr GetLoginCharacter(const int32 inCharacterID);
	LoginCharacterPtr GetLoginCharacter(const std::string& inCharacterName);

private:
	std::map<int32, LoginCharacterPtr> mCharacters;
};

