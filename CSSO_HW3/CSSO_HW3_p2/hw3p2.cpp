#include "stdafx.h"
#include <windows.h>

HANDLE hEvent, hEvent2;

int a = 1, b = 2;

DWORD WINAPI write_number(LPVOID lpParam);

DWORD WINAPI read_number(LPVOID lpParam);

int _tmain(int argc, _TCHAR* argv[])
{
	hEvent = CreateEvent(NULL, false, false, NULL);
	hEvent2 = CreateEvent();

	HANDLE hThread1, hThread2;

	hThread1 = CreateThread(NULL, NULL, write_number, NULL, NULL, NULL);
	hThread2 = CreateThread(NULL, NULL, read_number, NULL, NULL, NULL);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	CloseHandle(hThread1);
	CloseHandle(hThread2);

	CloseHandle(hEvent);
	CloseHandle(hEvent2);
	return 0;
}

DWORD WINAPI write_number(LPVOID lpParam)
{
	for (int i = 0; i<1000; i++)
	{
		WaitForSingleObject(hEvent2, INFINITE);
		a = rand() % 1000;
		b = a * 2;
		printf("Generate\n");
		SetEvent(hEvent);
	}
	return 0;
}

DWORD WINAPI read_number(LPVOID lpParam)
{

	for (int i = 0; i<1000; i++)
	{
		WaitForSingleObject(hEvent, INFINITE);
		if (b != 2 * a)
		{
			printf("Wrong!\n");
			exit(-1);
		}
		printf("Right!\n");
		SetEvent(hEvent2);
	}

	return 0;
}