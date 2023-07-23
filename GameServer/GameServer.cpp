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

void Func(KDTree& kdTree)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(45.0f, 55.0f);

	std::vector<Location> locations;
	for (int32 index = 0; index < kdTree.GetKDTreeSize(); index++)
	{
		locations.push_back(Location(dis(gen), dis(gen), 50.0f));
	}

	TimeStamp	time(L"KD_Tree");

	printf("CREATE NODE ");
	time.StartTimeStamp();
	for (int32 index = 0; index < kdTree.GetKDTreeSize(); index++)
	{
		kdTree.InsertNode(locations.at(index), index + 1, 1);
	}
	std::cout << time.GetTimeStamp() << std::endl << std::endl;

	time.StartTimeStamp();
	printf("FIND NODE ");
	std::vector<int64> gameObject;
	kdTree.FindNodes(FVector(50.0f, 50.0f, 50.0f), 50.0f, 1, gameObject);
	std::cout << time.GetTimeStamp() << std::endl;
	printf("FIND NODES COUNT : %lld\n\n", gameObject.size());

	time.StartTimeStamp();
	printf("DELETE NODE ");
	kdTree.DeleteTree();
	std::cout << time.GetTimeStamp() << std::endl << std::endl;
}

int main(void)
{
	StartServer();
	system("pause");
	return 0;
}