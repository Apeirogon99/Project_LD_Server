#include "pch.h"
#include "IdentityManager.h"

IdentityManager::IdentityManager() : GameObject(L"IdentityManager"), mToken(""), mGlobalID(0), mServerID(0)
{
}

IdentityManager::~IdentityManager()
{
}

void IdentityManager::OnInitialization()
{
}

void IdentityManager::OnDestroy()
{
}

void IdentityManager::OnTick(const int64 inDeltaTime)
{
}

bool IdentityManager::IsValid()
{
	return (this->GetOwner().lock() != nullptr);
}

void IdentityManager::SetToken(const std::string& inToken)
{
	mToken = inToken;
}

void IdentityManager::SetGlobalID(const int32& inGlobalID)
{
	mGlobalID = inGlobalID;
}

void IdentityManager::SetServerID(const int32& inServerID)
{
	mServerID = inServerID;
}
