#pragma once

#define ErrorToInt(x) static_cast<int32>(x)

enum class EGameErrorType : int32
{
	FAILURE = 0,
	SUCCESS = 1,

	INVALID_TOKEN,

};

static std::string GameErrorToString(int32 error)
{
	EGameErrorType type = static_cast<EGameErrorType>(error);

	switch (type)
	{
	case EGameErrorType::FAILURE:
		return std::string("FAILURE");
		break;
	case EGameErrorType::SUCCESS:
		return std::string("SUCCESS");
		break;
	case EGameErrorType::INVALID_TOKEN:
		return std::string("INVALID_TOKEN");
		break;
	default:
		return std::string("UNKNOWN_ERROR");
		break;
	}

}