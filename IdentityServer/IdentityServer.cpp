#include "pch.h"
#include "IdentityPacket.pb.h"
#include <conio.h>

using namespace std;

void StopServer(IdentityServicePtr service);
void StartServer()
{
	IdentityServicePtr service = std::make_shared<IdentityService>();
	thread serviceThread = thread(StopServer, service);

	service->ServiceOpen();

	if (serviceThread.joinable())
	{
		serviceThread.join();
	}
}

void StopServer(IdentityServicePtr service)
{
	char input;

	while (true)
	{
		input = _getch();
		if (input == 'q')
		{
			service->ServiceClose();
			break;
		}
	}
}

int main(void)
{
	StartServer();
	system("pause");
}