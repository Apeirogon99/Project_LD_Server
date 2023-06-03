#include "pch.h"
#include "Ticket.h"

Ticket::Ticket()
{
	MakeTicket();
}

Ticket::~Ticket()
{
}

bool Ticket::MakeTicket()
{
	std::string ticket;
	GUID guid;
	size_t tcnt;
	int8 szlld[129] = { NULL, };
	WCHAR wszlld[129] = { NULL, };
	errno_t error;
	
	HRESULT result = CoCreateGuid(&guid);

	if (result != S_OK)
	{
		wprintf(L"GUID������ ���� �Ͽ����ϴ�.\n");
		return false;
	}
			
	result = StringFromGUID2(guid, wszlld, 128);
	if (result == 0)
	{
		wprintf(L"GUID��ȯ�� ���� �Ͽ����ϴ�.\n");
		return false;
	}
		
	error = ::wcstombs_s(&tcnt, szlld, wszlld, 128);
	if (error != 0)
	{
		wprintf(L"GUID MBSC ��ȯ�� ���� �Ͽ����ϴ�.\n");
		return false;
	}
		
	ticket = szlld;
	ticket.erase(ticket.begin());
	ticket.erase(ticket.end() - 1);
		
	//wprintf(L"GUID ���� �Ϸ�.\n");

	mTicket = ticket;
	return true;
}

std::string Ticket::GetTicket()
{
	return mTicket;
}
