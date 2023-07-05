#pragma once

class PacketUtils
{
public:
	static Transform			ToFTransform(const Protocol::STransform& inTransform);
	static Protocol::STransform	ToSTransform(const Transform& inTransform);

	static FVector				ToFVector(const Protocol::SVector& inVector);
	static Protocol::SVector	ToSVector(const FVector& inVector);

	static FRotator				ToFRotator(const Protocol::SRotator& inRotator);
	static Protocol::SRotator	ToSRotator(const FRotator& inRotator);
};