#pragma once

enum class EServerType : int32
{
	UNKNOWN_SERVER		= 0,
	IDENTITY_SERVER		= 1,
	GAME_SERVER			= 2,
};

enum class ECharacterClass : int32
{
	UNKNOWN_CLASS = 0,
	WARRIOR_CLASS = 1,
	WIZARD_CLASS = 2,
};

enum class EPlayerMode : int32
{
	Unspecified_MODE = 0,
	Move_MODE,
	PickUp_MODE,
	Attack_MODE,
	Skill_MODE,
};