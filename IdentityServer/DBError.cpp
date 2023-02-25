#include "pch.h"

std::wstring DBError::GetDBErrorToString(ESPResult result)
{
	switch (result)
	{
	case ESPResult::Success:
		return L"���������� SP�� �����";
		break;
	case ESPResult::Failed:
		return L"SP�� �����Ͽ����ϴ�";
		break;
	case ESPResult::Close:
		return L"Record�� ���� �ֽ��ϴ�";
		break;
	case ESPResult::Index_None:
		return L"���ϴ� ���� ã�� �� �����ϴ�";
		break;
	default:
		return L"�� �� ���� �����Դϴ�";
		break;
	}
    return  L"�� �� ���� �����Դϴ�";
}
