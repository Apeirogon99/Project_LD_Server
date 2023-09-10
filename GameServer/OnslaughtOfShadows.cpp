#include "pch.h"
#include "OnslaughtOfShadows.h"

OnslaughtOfShadows::OnslaughtOfShadows() : EnemyAttack(L"OnslaughtOfShadows"), mActive(false)
{
	this->mDefaultCollisionComponent = new BoxCollisionComponent;
}

OnslaughtOfShadows::~OnslaughtOfShadows()
{
}

void OnslaughtOfShadows::OnInitialization()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	SetTick(true, SYSTEM_TICK);

	BoxCollisionComponent* collision = GetBoxCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(200.0f, 100.0f, 100.0f));

	this->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Place);
	this->SetDamage(100.0f);
}

void OnslaughtOfShadows::OnDestroy()
{
	Protocol::S2C_DisAppearGameObject disappearGameObjectPacket;
	disappearGameObjectPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disappearGameObjectPacket);
	this->BrodcastPlayerViewers(sendBuffer);
}

void OnslaughtOfShadows::OnTick(const int64 inDeltaTime)
{
	if (false == IsValid())
	{
		return;
	}

	if (mActive)
	{
		if (this->IsLife(inDeltaTime))
		{
			GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
			if (nullptr == world)
			{
				return;
			}
			const int64& worldTime = world->GetWorldTime();
			const int64& nextWorldTime = world->GetNextWorldTime();

			ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
			std::shared_ptr<EnemyRich> rich = std::static_pointer_cast<EnemyRich>(this->GetOwner().lock());
			if (nullptr == rich)
			{
				return;
			}

			rich->PushTask(worldTime, &EnemyRichPhase3::DoTeleportLocation, mOriginLocation);

			world->PushTask(nextWorldTime, &GameWorld::DestroyActor, this->GetGameObjectID());
			return;
		}

		this->CheackCollision();
	}
}

bool OnslaughtOfShadows::IsValid()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return false;
	}

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{

		if (false == world->IsValidActor(this->GetGameObjectID()))
		{
			return false;
		}

		world->PushTask(world->GetNextWorldTime(), &GameWorld::DestroyActor, this->GetGameObjectID());
	}

	return true;
}

void OnslaughtOfShadows::OnAppearActor(ActorPtr inAppearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inAppearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == IsValid())
	{
		return;
	}

	if (mPlayerViewers.find(anotherPlayerState) != mPlayerViewers.end())
	{
		return;
	}
	this->InsertPlayerViewer(anotherPlayerState);
	anotherPlayerState->InsertActorMonitor(this->GetActorPtr());

	WorldPtr world = this->GetWorld().lock();
	if (nullptr == world)
	{
		assert(!world);
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}

	Protocol::S2C_AppearSkill appearSkillPacket;
	appearSkillPacket.set_remote_id(owner->GetGameObjectID());
	appearSkillPacket.set_object_id(this->GetGameObjectID());
	appearSkillPacket.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Onslaught_Of_Shadows));
	appearSkillPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(this->GetLocation()));
	appearSkillPacket.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(this->GetRotation()));
	appearSkillPacket.set_duration(worldTime);

	SendBufferPtr appearSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, appearSkillPacket);
	anotherPlayerState->Send(appearSendBuffer);
}

void OnslaughtOfShadows::OnDisAppearActor(ActorPtr inDisappearActor)
{
	PlayerCharacterPtr anotherPlayerCharacter = std::static_pointer_cast<PlayerCharacter>(inDisappearActor);
	if (nullptr == anotherPlayerCharacter)
	{
		return;
	}

	RemotePlayerPtr anotherRemotePlayer = std::static_pointer_cast<RemotePlayer>(anotherPlayerCharacter->GetOwner().lock());
	if (nullptr == anotherRemotePlayer)
	{
		return;
	}

	PlayerStatePtr anotherPlayerState = std::static_pointer_cast<PlayerState>(anotherRemotePlayer->GetRemoteClient().lock());
	if (nullptr == anotherPlayerState)
	{
		return;
	}

	if (false == this->FindPlayerViewer(anotherPlayerState))
	{
		return;
	}
	this->ReleasePlayerViewer(anotherPlayerState);
	anotherPlayerState->ReleaseActorMonitor(this->GetActorPtr());

	Protocol::S2C_DisAppearGameObject disAppearItemPacket;
	disAppearItemPacket.set_object_id(this->GetGameObjectID());

	SendBufferPtr appearItemSendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, disAppearItemPacket);
	anotherPlayerState->Send(appearItemSendBuffer);
}

void OnslaughtOfShadows::SetOnslaughtOfShadows(FVector inLU, FVector inRU, FVector inRD, FVector inLD, FVector inOriginLocation)
{
	Plane planes[4];
	float lineLenght = FVector::Distance(inLU, inRU);
	float diagonalLenght = std::sqrtf(std::powf(FVector::Distance(inLU, inRU), 2.0f) + std::powf(FVector::Distance(inLU, inLD), 2.0f)) / 2.0f;

	planes[0]	= Plane(FVector(inLU.GetX(), inLU.GetY(), inLU.GetZ() - 200.0f), FVector(inRU.GetX(), inRU.GetY(), inRU.GetZ() + 200.0f));
	planes[1]	= Plane(FVector(inRD.GetX(), inRD.GetY(), inRD.GetZ() - 200.0f), FVector(inRU.GetX(), inRU.GetY(), inRU.GetZ() + 200.0f));
	planes[2]	= Plane(FVector(inLU.GetX(), inLU.GetY(), inLU.GetZ() - 200.0f), FVector(inLD.GetX(), inLD.GetY(), inLD.GetZ() + 200.0f));
	planes[3]	= Plane(FVector(inRD.GetX(), inRD.GetY(), inRD.GetZ() - 200.0f), FVector(inLD.GetX(), inLD.GetY(), inLD.GetZ() + 200.0f));
	lineLenght	= FVector::Distance(inLU, inRU);

	mOriginLocation = inOriginLocation;

	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	std::shared_ptr<EnemyRichPhase3> lich = std::static_pointer_cast<EnemyRichPhase3>(owner);
	if (nullptr == lich)
	{
		return;
	}

	FVector	 location = this->GetLocation();
	FRotator rotation = this->GetRotation();
	FVector	 foward = rotation.GetForwardVector();
	FVector backward = FRotator::TurnRotator(rotation).Vector();

	FVector fowardLoation	 = location + (foward * lineLenght);
	FVector backwardLocation = location + (backward * lineLenght);

	FVector startLocation = FVector(10050.0f, 10050.0f, 200.0f);
	FVector endLocation = FVector(10050.0f, 10050.0f, 200.0f);

	const float debugDuration = 10.0f;

	for (int32 i = 0; i < 4; ++i)
	{
		Plane plane = planes[i];
		FVector p1 = FVector(plane.mP1.GetX(), plane.mP1.GetY(), plane.mP1.GetZ() + 200.0f);
		FVector p2 = FVector(plane.mP2.GetX(), plane.mP2.GetY(), plane.mP2.GetZ() - 200.0f);
		float pd = FVector::Distance(p1, p2) / 2.0f;
		PacketUtils::DebugDrawBox(this->GetPlayerViewers(), p1, p2, FVector(pd, 100.0f, 200.0f), debugDuration);
	}


	float lbd = FVector::Distance(location, backwardLocation) / 2.0f;
	PacketUtils::DebugDrawBox(this->GetPlayerViewers(), location, backwardLocation, FVector(lbd, 100.0f, 100.0f), debugDuration);
	for (int32 i = 0; i < 4; ++i)
	{
		Plane plane = planes[i];
		if (true == GetIntersection(location, backwardLocation, plane.mP1, plane.mP2, startLocation))
		{
			float distance = FVector::Distance(inOriginLocation, startLocation);
			if (distance <= diagonalLenght)
			{
				PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), startLocation, 100.0f, debugDuration);
				break;
			}
		}
	}

	float lod = FVector::Distance(location, fowardLoation) / 2.0f;
	PacketUtils::DebugDrawBox(this->GetPlayerViewers(), location, fowardLoation, FVector(lod, 100.0f, 100.0f), debugDuration);
	for (int32 i = 0; i < 4; ++i)
	{
		Plane plane = planes[i];
		if (true == GetIntersection(location, fowardLoation, plane.mP1, plane.mP2, endLocation))
		{
			float distance = FVector::Distance(inOriginLocation, endLocation);
			if (distance <= diagonalLenght)
			{
				PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), endLocation, 100.0f, debugDuration);
				break;
			}
		}
	}

	PacketUtils::DebugDrawSphere(this->GetPlayerViewers(), location, 100.0f, debugDuration);

	{
		float speed = 600.0f;
		float distance = FVector::Distance(startLocation, endLocation);
		float time = distance / speed;
		int64 duration = static_cast<int64>(time * 1000.0f);
		lich->DoMoveLocation(startLocation, endLocation, duration, speed);
		this->ReserveDestroy(duration);
	}

	{
		//Protocol::S2C_ReactionSkill reactionSkill;
		//reactionSkill.set_remote_id(owner->GetGameObjectID());
		//reactionSkill.set_object_id(this->GetGameObjectID());
		//reactionSkill.set_skill_id(static_cast<int32>(ESkillID::Skill_Rich_Onslaught_Of_Shadows));
		//reactionSkill.mutable_location()->CopyFrom(PacketUtils::ToSVector(startLocation));
		//reactionSkill.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(rotation));
		//reactionSkill.set_duration(worldTime);
		//
		//SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, reactionSkill);
		//this->BrodcastPlayerViewers(sendBuffer);
	}

	mActive = true;
}

void OnslaughtOfShadows::CheackCollision()
{
	GameWorldPtr world = std::static_pointer_cast<GameWorld>(GetWorld().lock());
	if (nullptr == world)
	{
		return;
	}
	const int64& worldTime = world->GetWorldTime();

	ActorPtr owner = std::static_pointer_cast<Actor>(this->GetOwner().lock());
	if (nullptr == owner)
	{
		return;
	}
	FVector	 location	= owner->GetLocation();
	FRotator rotation	= owner->GetRotation();

	FVector	 foward		= rotation.GetForwardVector();
	FVector	 right		= rotation.GetRightVector();

	location = location + (foward * 90.0f);

	FVector	 boxExtent = this->GetBoxCollisionComponent()->GetBoxCollision().GetBoxExtent();
	Location boxStartLocation	= location + (right * +boxExtent.GetX());
	Location boxEndLocation		= location + (right * -boxExtent.GetX());
	Location boxCenterLocation	= location;
	BoxTrace boxTrace(owner, boxStartLocation, boxEndLocation, true, boxExtent, rotation);

	const float debugDuration = 0.2f;
	PacketUtils::DebugDrawBox(this->GetPlayerViewers(), boxStartLocation, boxEndLocation, boxExtent, debugDuration);

	std::vector<ActorPtr> findActors;
	uint8 findActorType = static_cast<uint8>(EActorType::Player);
	bool result = world->FindActors(boxTrace, findActorType, findActors);
	for (ActorPtr actor : findActors)
	{
		if (nullptr == actor)
		{
			continue;
		}

		if (false == actor->IsValid())
		{
			continue;
		}

		const int64& gameObjectID = actor->GetGameObjectID();
		auto find = mOverlapGameObjectID.find(gameObjectID);
		if (find != mOverlapGameObjectID.end())
		{
			continue;
		}

		mOverlapGameObjectID.insert(gameObjectID);
		actor->PushTask(worldTime, &Actor::OnHit, owner, this->GetDamage());
	}

}

void OnslaughtOfShadows::OnParrying(ActorPtr inActor)
{
}

bool OnslaughtOfShadows::GetIntersection(FVector inP1, FVector inP2, FVector inQ1, FVector inQ2, FVector& outIntersection)
{

	FVector D = inP2 - inP1;
	FVector N = (inQ2 - inQ1).Normalize();

	float dot = FVector::DotProduct(D, N);

	float t = FVector::DotProduct(N, (inQ1 - inP1)) / dot;

	FVector I = inP1 + D * t;

	if (t >= 0.0f && t <= 1.0f) 
	{
		outIntersection = I;
		return true;
	}

	return false;
}

BoxCollisionComponent* OnslaughtOfShadows::GetBoxCollisionComponent()
{
	return static_cast<BoxCollisionComponent*>(this->GetDefaultCollisionComponent());
}