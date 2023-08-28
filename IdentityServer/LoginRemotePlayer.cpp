#include "pch.h"
#include "RemotePlayer.h"
#include "Character.h"

LoginRemotePlayer::LoginRemotePlayer() : RemotePlayer()
{

}

LoginRemotePlayer::~LoginRemotePlayer()
{

}

void LoginRemotePlayer::OnInitialization()
{
	IdentityTaskPtr taskManager = std::static_pointer_cast<IdentityTask>(this->GetTaskManagerRef().lock());
	if (nullptr == taskManager)
	{
		return;
	}

	LoginWorldPtr world = taskManager->GetWorld();
	if (nullptr == world)
	{
		return;
	}

	GameObjectRef owner = this->GetGameObjectRef();

	mCharacterManager = std::make_shared<CharacterManager>();
	mCharacterManager->SetOwner(owner);
	taskManager->PushTask(this->GetCharacterManager()->GetGameObjectPtr());

	mRoomManager = std::make_shared<RoomManager>();
	mRoomManager->SetOwner(owner);
	taskManager->PushTask(this->GetRoomManager()->GetGameObjectPtr());

	mIdentityManager = std::make_shared<IdentityManager>();
	mIdentityManager->SetOwner(owner);
	taskManager->PushTask(this->GetIdentityManager()->GetGameObjectPtr());
}

void LoginRemotePlayer::OnDestroy()
{
	IdentityTaskPtr taskManager = std::static_pointer_cast<IdentityTask>(this->GetTaskManagerRef().lock());
	if (nullptr == taskManager)
	{
		return;
	}

	LoginWorldPtr world = taskManager->GetWorld();
	if (nullptr == world)
	{
		return;
	}

	taskManager->ReleaseTask(this->GetIdentityManager()->GetGameObjectPtr());
	mIdentityManager.reset();

	taskManager->ReleaseTask(this->GetRoomManager()->GetGameObjectPtr());
	mRoomManager.reset();

	taskManager->ReleaseTask(this->GetCharacterManager()->GetGameObjectPtr());
	mCharacterManager.reset();

}

void LoginRemotePlayer::OnTick(const int64 inDeltaTime)
{

}

bool LoginRemotePlayer::IsValid()
{
	return (this->GetRemoteClient().lock() != nullptr);
}