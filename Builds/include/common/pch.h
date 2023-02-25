#pragma once

#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#pragma comment(lib, "Builds\\debug\\ApeirogonLibrary.lib")
#else
#pragma comment(lib, "Builds\\release\\ApeirogonLibrary.lib")
#endif

#include "ApeirogonLibrary.h"
#include "CommonLibaray.h"