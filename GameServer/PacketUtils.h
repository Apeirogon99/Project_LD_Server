#pragma once

#define DEBUG_DRAW 1

class PacketUtils
{
public:
	static Transform			ToFTransform(const Protocol::STransform& inTransform);
	static Protocol::STransform	ToSTransform(const Transform& inTransform);

	static FVector				ToFVector(const Protocol::SVector& inVector);
	static Protocol::SVector	ToSVector(const FVector& inVector);

	static FRotator				ToFRotator(const Protocol::SRotator& inRotator);
	static Protocol::SRotator	ToSRotator(const FRotator& inRotator);

	static void DebugDrawBox(RemoteClientPtr inClient, const FVector& inStart, const FVector& inEnd, const FVector& inExtent, const float& inDuration);
	static void DebugDrawBox(const std::set<RemoteClientPtr>& inClients, const FVector& inStart, const FVector& inEnd, const FVector& inExtent, const float& inDuration);
	static void DebugDrawSphere(RemoteClientPtr inClient, const FVector& inLocation, const float& inRadius, const float& inDuration);
	static void DebugDrawSphere(const std::set<RemoteClientPtr>& inClients, const FVector& inLocation, const float& inRadius, const float& inDuration);
};