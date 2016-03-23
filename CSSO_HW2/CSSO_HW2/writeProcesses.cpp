/*Creati 2 programe:
1. Primul va enumera toate procesele din sistem care au mai putin de 3 fire de
executie si le va transmite, printr - un fisier mapat in memorie, programului 2
2. Al doilea program, la initializare, isi va seta privilegiul SE_DEBUG_NAME, va
citi din fisierul mapat in memorie procesele transmise de programul 1 si le va omori pe
toate.
*/

#include <windows.h>
#include "stdafx.h"
#include "TlHelp32.h"

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
BOOL getProcessList();
void printError(TCHAR* msg);
HANDLE createMapFile();
LPDWORD mapViewFile(HANDLE hMapFile);

///main function
void __cdecl _tmain(int argc, TCHAR *argv[])
{
	getProcessList();
	return;
}

BOOL getProcessList() {

	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	DWORD* pBuf;

	pBuf = mapViewFile(createMapFile());

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot (of processes)"));
		return(FALSE);
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		printError(TEXT("Process32First")); // show cause of failure
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return(FALSE);
	}
	int count = 0;
	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		if (pe32.cntThreads < 3) {
		_tprintf(TEXT("\n\n====================================================="));
		_tprintf(TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile);
		_tprintf(TEXT("\n-------------------------------------------------------"));

		// Retrieve the priority class.
		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess == NULL)
			printError(TEXT("OpenProcess"));
		else
		{
			dwPriorityClass = GetPriorityClass(hProcess);
			if (!dwPriorityClass)
				printError(TEXT("GetPriorityClass"));

			pBuf[count] = pe32.th32ProcessID;
			count++;
			CloseHandle(hProcess);

		}
		
			_tprintf(TEXT("\n  Process ID        = 0x%08X"), pe32.th32ProcessID);
			_tprintf(TEXT("\n  Thread count      = %d"), pe32.cntThreads);
			_tprintf(TEXT("\n  Parent process ID = 0x%08X"), pe32.th32ParentProcessID);
			_tprintf(TEXT("\n  Priority base     = %d"), pe32.pcPriClassBase);
			if (dwPriorityClass)
				_tprintf(TEXT("\n  Priority class    = %d"), dwPriorityClass);

		}

	} while (Process32Next(hProcessSnap, &pe32));
	
	_tprintf(TEXT("\nPress any key to close.\n"));
	int c;
	c = getchar();
	UnmapViewOfFile(pBuf);
	CloseHandle(hProcessSnap);
	return(TRUE);
}

HANDLE createMapFile() {
	HANDLE hMapFile;
	DWORD* pBuf;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return NULL;
	}else
	return hMapFile;
}

LPDWORD mapViewFile(HANDLE hMapFile) {
	LPDWORD pBuf;

	pBuf = (DWORD*)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
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