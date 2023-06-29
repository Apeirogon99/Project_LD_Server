#pragma once

class PacketUtils
{
public:
	static FVector				ToFVector(const Protocol::SVector& inVector);
	static Protocol::SVector	ToSVector(const FVector& inVector);

	static FRotator				ToFRotator(const Protocol::SRotator& inRotator);
	static Protocol::SRotator	ToSRotator(const FRotator& inRotator);
};