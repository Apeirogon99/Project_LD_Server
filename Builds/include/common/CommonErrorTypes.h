#pragma once

//0000 ~ 1000 = 공통
//1000 ~ 2000 = 계정
//2000 ~ 3000 = 게임

enum class EDCommonErrorType
{
	FAILURE = -1,
	SUCCESS = 0,
	INDEX_NONE = 1,
	SMTP = 25,

	NOT_EXISTS_ID = 1001,
	ID_DISTINCT = 1002,
	EMAIL_DISTINCT = 1003,
	EMAIL_NOT_VERIFY = 1004,
	ALREADY_EMAIL_VERIFY = 1005,
	NOT_SAME_VERIFY = 1006,
	TEMP_VERIFY = 1007,

	CHARACTER_MAX = 2001,
	POSTION_DISTINCT = 2002,
	NAME_DISTINCT = 2003,
	CHARACTER_NONE = 2004,

	//Friend
	NOT_EXISTS_FRIEND_NAME					= 4001,	//존재하지 않는 이름
	NOT_FRIEND_WITH_PLAYER					= 4002,	//플레이어와 친구가 아님
	ALREADY_FRIEND_WITH_PLAYER				= 4003, //이미 친구임
	ALREADY_FRIEND_IS_BLOCK					= 4004, //이미 블락된 상태임
	ALREADY_FRIEND_IS_REQUEST				= 4005, //이미 요청을 보낸 상태임
	INVALID_LIST_TYPE						= 4009, //없는 리스트 타입
	ALREADY_SAME_ACTION						= 4010, //이미 같은 행동을 하고 있음
};

static std::string GetNetworkError(int32 error)
{
	EDCommonErrorType type = static_cast<EDCommonErrorType>(error);

	switch (type)
	{
	case EDCommonErrorType::FAILURE:
		return std::string("알 수 없는 에러");
		break;
	case EDCommonErrorType::SUCCESS:
		return std::string("성공");
		break;
	case EDCommonErrorType::INDEX_NONE:
		return std::string("해당 인덱스가 존재하지 않습니다.");
		break;
	case EDCommonErrorType::SMTP:
		return std::string("SMTP 에러");
		break;
	case EDCommonErrorType::NOT_EXISTS_ID:
		return std::string("아이디가 존재하지 않습니다.");
		break;
	case EDCommonErrorType::ID_DISTINCT:
		return std::string("해당 아이디가 존재합니다.");
		break;
	case EDCommonErrorType::EMAIL_DISTINCT:
		return std::string("해당 이메일이 존재합니다.");
		break;
	case EDCommonErrorType::EMAIL_NOT_VERIFY:
		return std::string("이메일 인증이 되어있지 않습니다.");
		break;
	case EDCommonErrorType::ALREADY_EMAIL_VERIFY:
		return std::string("이미 인증을 하였습니다.");
		break;
	case EDCommonErrorType::NOT_SAME_VERIFY:
		return std::string("인증 번호가 다릅니다.");
		break;
	case EDCommonErrorType::TEMP_VERIFY:
		return std::string("임시 임증되었습니다");
		break;
	case EDCommonErrorType::CHARACTER_MAX:
		return std::string("생성할 수 있는 캐릭터를 초과하였습니다.");
		break;
	case EDCommonErrorType::POSTION_DISTINCT:
		return std::string("이미 같은 자리에 캐릭터가 존재합니다.");
		break;
	case EDCommonErrorType::NAME_DISTINCT:
		return std::string("이미 존재하는 이름입니다.");
		break;
	case EDCommonErrorType::CHARACTER_NONE:
		return std::string("캐릭터가 존재하지 않습니다.");
		break;
	case EDCommonErrorType::NOT_EXISTS_FRIEND_NAME:
		return std::string("존재하지 않는 이름입니다.");
		break;
	case EDCommonErrorType::NOT_FRIEND_WITH_PLAYER:
		return std::string("플레이와 친구가 아닌 상태입니다");
		break;
	case EDCommonErrorType::ALREADY_FRIEND_WITH_PLAYER:
		return std::string("이미 친구인 상태입니다.");
		break;
	case EDCommonErrorType::ALREADY_FRIEND_IS_BLOCK:
		return std::string("이미 차단되어 있는 상태입니다.");
		break;
	case EDCommonErrorType::ALREADY_FRIEND_IS_REQUEST:
		return std::string("이미 요청을 보낸 상태입니다.");
		break;
	case EDCommonErrorType::INVALID_LIST_TYPE:
		return std::string("올바른 리스트 타입이 아닙니다.");
		break;
	case EDCommonErrorType::ALREADY_SAME_ACTION:
		return std::string("이미 처리 되어있습니다.");
		break;
	default:
		return std::string("알 수 없는 에러");
		break;
	}
}

static int32 GetDatabaseErrorToInt(const EDCommonErrorType& inErrorType)
{
	return static_cast<int32>(inErrorType);
}