#include "pch.h"
#include "Character.h"

Character::Character(const RemotePlayerRef& inReomtePlayer) : GameObject(L"Character"), mRemotePlayer(inReomtePlayer)
{

}

Character::~Character()
{

}

void Character::MoveDestination(RemotePlayerPtr inReomtePlayer, Protocol::STransform inWorldPosition)
{
	//inReomtePlayer->GetCharacter()->mWorldPosition = inWorldPosition;
}
