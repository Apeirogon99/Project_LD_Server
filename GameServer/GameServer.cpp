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

	float ma[4][4]
	{
		{2, 6, 3, -2},
		{4, 5, 1, 1},
		{5, 2, 4, 7},
		{0, 0, 0, 1}
	};

	float ma2[4][4]
	{
		{3, 6, 2, 5},
		{1, 5, 4, 5},
		{4, 2, 5, 5},
		{0, 0, 0, 1}
	};
	Matrix m(ma);
	Matrix m2(ma2);

	Matrix mul = Matrix::Inverse(m) * m2;
	mul.ToString();

	const double cutoff = 0.999999;
	bool existsParallelPair = false;

	PlayerCharacterPtr actor = std::make_shared<PlayerCharacter>();
	actor->SetLocation(FVector(200.0f, 0.0f, 478.0f));
	actor->SetRotation(FRotator(0.0f, 0.0f, 0.0f));
	
	//0		= -45 ~ 58
	//45	= -39 ~ 54

	BoxCollisionComponent component;
	component.SetOwner(actor->GetActorRef());
	component.SetBoxCollision(FVector(42.0f, 96.0f, 42.0f));

	PlayerCharacterPtr traceActor = std::make_shared<PlayerCharacter>();
	traceActor->SetLocation(FVector(0.0f, 0.0f, 486.0f));
	traceActor->SetRotation(FRotator(0.0f, 0.0f, 0.0f));

	FVector		boxExtent = FVector(300.0f, 100.0f, 100.0f);
	const float collision = 42.0f;

	for (float i = -180; i < 180;)
	{

		traceActor->SetRotation(FRotator(0.0f, static_cast<float>(i), 0.0f));

		FVector	 location = traceActor->GetLocation();
		FRotator rotation = traceActor->GetRotation();

		FVector	 foward = rotation.GetForwardVector();
		Location boxStartLocation = location + (foward * collision);
		Location boxEndLocation = boxStartLocation + (foward * (boxExtent.GetX() * 2.0f));

		BoxTrace trace(traceActor->GetActorRef(), boxStartLocation, boxEndLocation, false, boxExtent, traceActor->GetRotation());
		bool result = trace.BoxCollisionTraceOBB(component);

		if (result)
		{
			printf("\t\t[%+.3f] -> %s\n", rotation.GetYaw(), (result == true) ? "CHEACK" : "FALSE");
		}
		else
		{
			printf("\t[%+.3f]\n", rotation.GetYaw());
		}
		i = i + 0.1;
	}

	//StartServer();
	system("pause");
	return 0;
}