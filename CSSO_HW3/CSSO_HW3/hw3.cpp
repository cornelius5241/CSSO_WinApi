#include <stdlib.h>    
#include <windows.h>
#include "stdafx.h"

CRITICAL_SECTION criticalSection;
int a, b;

DWORD WINAPI write_number(LPVOID lpParam);

DWORD WINAPI read_number(LPVOID lpParam);

int _tmain(int argc, _TCHAR* argv[])
{

	HANDLE hThread1, hThread2;
	InitializeCriticalSection(&criticalSection);

	hThread1 = CreateThread(NULL, NULL, write_number, NULL, NULL, NULL);
	hThread2 = CreateThread(NULL, NULL, read_number, NULL, NULL, NULL);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	DeleteCriticalSection(&criticalSection);

	CloseHandle(hThread1);
	CloseHandle(hThread2);	

	return 0;
}


DWORD WINAPI write_number(LPVOID lpParam)
{
	for (int i = 0; i<1000; i++)
	{
		EnterCriticalSection(&criticalSection);
		a = rand() % 1000;
		b = a * 2;
		_tprintf(TEXT("Generate\n"));
		LeaveCriticalSection(&criticalSection);
	}
	return 0;
}

DWORD WINAPI read_number(LPVOID lpParam)
{
	for (int i = 0; i<1000; i++)
	{
		EnterCriticalSection(&criticalSection);
		if (b != 2 * a)
		{
			_tprintf(TEXT("Wrong!!\n"));
			exit(-1);
		}
		_tprintf(TEXT("Right!\n"));
		LeaveCriticalSection(&criticalSection);
	}
	return 0;
}
