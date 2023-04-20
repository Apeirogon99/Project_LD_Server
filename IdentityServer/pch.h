#pragma once

#define WIN32_LEAN_AND_MEAN

#pragma warning ( disable : 4251 )

//Protocol buffer
#ifdef _DEBUG
#pragma comment(lib, "Builds\\debug\\libprotobufd.lib")
#pragma comment(lib, "Builds\\debug\\libprotobuf-lited.lib")
#pragma comment(lib, "Builds\\debug\\libprotocd.lib")
#else
#pragma comment(lib, "Builds\\release\\libprotobuf.lib")
#pragma comment(lib, "Builds\\release\\libprotobuf-lite.lib")
#pragma comment(lib, "Builds\\release\\libprotoc.lib")
#endif

//Library
#ifdef _DEBUG
#pragma comment(lib, "Builds\\debug\\ApeirogonLibrary.lib")
#pragma comment(lib, "Builds\\debug\\CommonLibrary.lib")
#else
#pragma comment(lib, "Builds\\release\\ApeirogonLibrary.lib")
#pragma comment(lib, "Builds\\release\\CommonLibrary.lib")
#endif

#include "ApeirogonLibrary.h"
#include "CommonLibaray.h"
#include "IdentityLibaray.h"