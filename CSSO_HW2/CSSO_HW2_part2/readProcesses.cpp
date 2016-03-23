

#include "stdafx.h"
#include <windows.h>

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

void readAndClose();
HANDLE openFileMap();
LPDWORD mapViewFile(HANDLE hMapFile);
BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
	);
void printError(TCHAR* msg);
///main function
void __cdecl _tmain(int argc, TCHAR *argv[])
{
	HANDLE hToken = NULL;
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken);
	SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	readAndClose();
	CloseHandle(hToken);
	return;
}

void readAndClose() {

	LPDWORD pBuf;
	pBuf = mapViewFile(openFileMap());

	int count = 0;
	do {

		HANDLE hProcess = NULL;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pBuf[count]);
		if (hProcess == NULL)
			printError(TEXT("OpenProcess"));
		else
		{
			TerminateProcess(hProcess, 9);
			CloseHandle(hProcess);
		}

		count++;
	} while (pBuf[count] != NULL);

	UnmapViewOfFile(pBuf);

	return;
}

HANDLE openFileMap() {
	HANDLE hMapFile;

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return NULL;
	}
	else return hMapFile;
}

LPDWORD mapViewFile(HANDLE hMapFile) {
	LPDWORD pBuf;

	pBuf = (DWORD*)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return NULL;
	}
	else return pBuf;
}

BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
	)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

void printError(TCHAR* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; } while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Display the message
	_tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
}