#pragma once
#include <random>

class Random
{
public:
	APEIROGON_API static const double GetNormalDistribution(const double& inAverage, const double& inStandardDeviation);

private:
	static std::random_device	mRandomDevice;
	static std::mt19937			mMersenneTwister;
};