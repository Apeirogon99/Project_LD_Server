#include "pch.h"
#include "WinDump.h"
#include <DbgHelp.h>

const WCHAR* WinDump::mDumpPath = L"NONE";

WinDump::WinDump()
{
	::SetUnhandledExceptionFilter(ExceptionFilter);
	//wprintf(L"WinDump::WinDump() : set window dump\n");
}

WinDump::~WinDump()
{
}

void WinDump::Init(const WCHAR* inDumpPath)
{
	mDumpPath = inDumpPath;
	::SetUnhandledExceptionFilter(ExceptionFilter);
	wprintf(L"[WinDump::Init()] : WinDump is running(Path=%ws)\n", mDumpPath);
}

LONG __stdcall WinDump::ExceptionFilter(_EXCEPTION_POINTERS* execptionPointer)
{
	std::wstring dumpFilePath = mDumpPath;
	dumpFilePath += Time::NowTime(false);
	dumpFilePath += L".dmp";

	//���α׷� ������ ���ۺ��� �Ǵ� ������ �� ���¿��� ���� �ִ� ��ü�� ���� ������ �����մϴ�(����� �������� �ش�)
	_CrtMemDumpAllObjectsSince(NULL);

	HMODULE dumpModule = nullptr;
	dumpModule = ::LoadLibraryW(L"DBGHELP.DLL");
	if (nullptr == dumpModule)
	{
		wprintf(L"WinDump::ExceptionFilter() : Invalid dump dll\n");
		return 0;
	}

	HANDLE dumpFile = CreateFileW(dumpFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == dumpFile)
	{
		wprintf(L"WinDump::ExceptionFilter() : Invalid dump file\n");
		return 0;
	}

	_MINIDUMP_EXCEPTION_INFORMATION info;
	info.ThreadId = GetCurrentThreadId();
	info.ExceptionPointers = execptionPointer;
	info.ClientPointers = NULL;


	HANDLE currentProcess = GetCurrentProcess();
	DWORD currentProcessId = GetCurrentProcessId();
	_MINIDUMP_TYPE dumpType = MiniDumpNormal;

	
	WRITERDUMP dumpFunc = (WRITERDUMP)::GetProcAddress(dumpModule, "MiniDumpWriteDump");
	BOOL dumpResult = dumpFunc(currentProcess, currentProcessId, dumpFile, dumpType ,&info, NULL, NULL);
	//BOOL dumpResult = MiniDumpWriteDump(dumpHandle, currentProcess, dumpFile, dumpType, &info, NULL, NULL);
	if (FALSE == dumpResult)
	{
		wprintf(L"WinDump::ExceptionFilter() : Failed to write dump\n");
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	::CloseHandle(dumpFile);

	return EXCEPTION_CONTINUE_SEARCH;
}