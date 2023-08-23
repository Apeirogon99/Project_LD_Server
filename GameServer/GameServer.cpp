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

	PlayerCharacterPtr actor = std::make_shared<PlayerCharacter>();
	actor->SetLocation(FVector(200.0f, 0.0f, 478.0f));
	actor->SetRotation(FRotator(0.0f, 180.0f, 0.0f));
	
	BoxCollisionComponent component;
	component.SetOwner(actor->GetActorRef());
	component.SetBoxCollision(FVector(42.0f, 96.0f, 42.0f));

	PlayerCharacterPtr traceActor = std::make_shared<PlayerCharacter>();
	traceActor->SetLocation(FVector(0.0f, 0.0f, 486.0f));
	traceActor->SetRotation(FRotator(0.0f, 0.0f, 0.0f));


	FVector		boxExtent = FVector(300.0f, 100.0f, 100.0f);
	const float collision = 42.0f;

	for (float i = -180; i < 180; i+=0.1)
	{

		traceActor->SetRotation(FRotator(0.0f, static_cast<float>(i), 0.0f));

		FVector	 location = traceActor->GetLocation();
		FRotator rotation = traceActor->GetRotation();

		FVector	 foward = rotation.GetForwardVector();
		Location boxStartLocation = location + (foward * collision);
		Location boxEndLocation = boxStartLocation + (foward * (boxExtent.GetX() * 2));

		BoxTrace trace(traceActor->GetActorRef(), boxStartLocation, boxEndLocation, false, boxExtent, traceActor->GetRotation());
		bool result = trace.BoxCollisionTraceOBB(component);

		if(result)
			printf("[%+.3f] RESULT = %s\n", rotation.GetYaw(), (result == true) ? "TRUE" : "FALSE");
	}

	//StartServer();
	system("pause");
	return 0;
}