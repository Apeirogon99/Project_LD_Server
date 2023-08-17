#include "pch.h"
#include "KeyboardComponent.h"
#include "SkillHandler.h"
#include "ItemHandler.h"

KeyboardComponent::KeyboardComponent()
{
}

KeyboardComponent::~KeyboardComponent()
{
}

void KeyboardComponent::LoadBindKeyAndAction(GameRemotePlayerRef inOwner, const std::map<int32, BindAction>& inKeyActions)
{
    mKeyActions = inKeyActions;
    mOwner = inOwner;
}

bool KeyboardComponent::SetBindKeyAndAction(const int32& inKey, const EBindActionType& inActionType, const int32& inActionID)
{
    auto findKey = mKeyActions.find(inKey);
    if (findKey != mKeyActions.end())
    {
        return false;
    }

    std::pair<int32, BindAction> keyAction = std::make_pair(inKey, BindAction(inActionType, inActionID));
    mKeyActions.insert(keyAction);
    return true;
}

void KeyboardComponent::PressedKey(int32 inKey)
{
    auto findKey = mKeyActions.find(inKey);
    if (findKey == mKeyActions.end())
    {
        return;
    }
    const BindAction& action = findKey->second;

    GameRemotePlayerPtr owner = mOwner.lock();
    if (nullptr == owner)
    {
        return;
    }

    bool result = false;
    switch (action.GetActionType())
    {
    case EBindActionType::Action_Unspecified:
        break;
    case EBindActionType::Action_Item:
        result = ItemHandler::HandlePacket(mOwner, action.GetActionID(), true);
        break;
    case EBindActionType::Action_Skill:
        result = SkillHandler::HandlePacket(mOwner, action.GetActionID(), true);
        break;
    default:
        break;
    }   
}

void KeyboardComponent::ReleaseKey(int32 inKey)
{
    auto findKey = mKeyActions.find(inKey);
    if (findKey == mKeyActions.end())
    {
        return;
    }
    const BindAction& action = findKey->second;

    GameRemotePlayerPtr owner = mOwner.lock();
    if (nullptr == owner)
    {
        return;
    }

    switch (action.GetActionType())
    {
    case EBindActionType::Action_Unspecified:
        break;
    case EBindActionType::Action_Item:
        ItemHandler::HandlePacket(mOwner, action.GetActionID(), false);
        break;
    case EBindActionType::Action_Skill:
        SkillHandler::HandlePacket(mOwner, action.GetActionID(), false);
        break;
    default:
        break;
    }

}

const std::map<int32, BindAction>& KeyboardComponent::GetKeyActions() const
{
    return mKeyActions;
}