#pragma once
#pragma warning(disable : 4251)

#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#pragma comment(lib, "Builds\\debug\\ApeirogonLibrary.lib")
#pragma comment(lib, "Builds\\debug\\CommonLibrary.lib")
#pragma comment(lib, "Builds\\debug\\libprotobufd.lib")
#pragma comment(lib, "Builds\\debug\\libprotobuf-lited.lib")
#pragma comment(lib, "Builds\\debug\\libprotocd.lib")
#else
#pragma comment(lib, "Builds\\release\\ApeirogonLibrary.lib")
#pragma comment(lib, "Builds\\release\\CommonLibrary.lib")
#pragma comment(lib, "Builds\\release\\libprotobuf.lib")
#pragma comment(lib, "Builds\\release\\libprotobuf-lite.lib")
#pragma comment(lib, "Builds\\release\\libprotoc.lib")
#endif

#include "ApeirogonLibrary.h"
#include "CommonLibaray.h"
#include "IdentityLibaray.h"