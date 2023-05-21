#include "pch.h"
#include <conio.h>

using namespace std;

void StopServer(GameServicePtr service);
void StartServer()
{
	GameServicePtr service = std::make_shared<GameService>();
	thread serviceThread = thread(StopServer, service);

	service->ServiceOpen();

	if (serviceThread.joinable())
	{
		serviceThread.join();
	}
}

void StopServer(GameServicePtr service)
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
	return 0;
}