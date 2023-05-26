#include "pch.h"
#include "RemotePlayer.h"
#include "Character.h"

RemotePlayer::RemotePlayer() : mToken(""), mRemoteID(-1), mGlobalID(-1), mServerID(-1), mRoomType(ERoomType::None)
{
	for (int i = 0; i < 4; i++)
	{
		mCharacters[i] = Character(i);
	}
}

RemotePlayer::~RemotePlayer()
{

}


Character* RemotePlayer::FindCharacter(const int8* name)
{
	for (int i = 0; i < 4; i++)
	{
		if (strcmp(mCharacters[i].mName, name) == 0)
		{
			return &mCharacters[i];
		}
	}

	return nullptr;
}
