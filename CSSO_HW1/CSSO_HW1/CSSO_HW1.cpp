// CSSO HW1.cpp : Data o cheie de registry creati pentru fiecare subcheie un director, si pentru fiecare
// valoare, un fisier.Fisierul va avea numele valorii, iar continutul fisierului va fi de forma :
//			tipData
//			valoare
//Programul trebuie sa fie cabail sa parcurga recursiv cheia de registry
//Corneliu Tutuianu - FII 2016 CSS0 IIIA5

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <string.h>
#include <string>

TCHAR* DIR="C:\\Users\\Cornelius\\Desktop\\TestDir";

HKEY openKey(TCHAR parent[], TCHAR path[], TCHAR keyName[]);
void beginCreateFiles(HKEY root_hKey, TCHAR* dir);
void createFile(TCHAR* path, TCHAR* name, BYTE* DataBuffer, DWORD dwBytesToWrite, DWORD type);

///main function, command line arguments are @HIVE @path and @key_name
void __cdecl _tmain(int argc, TCHAR *argv[])
{
	HKEY root_hKey = NULL;
	if (argc < 4) {
		_tprintf(TEXT("[Log _tmain] Start program. Arguments are missing:  \n"));
		_tprintf(TEXT("[Log _tmain] Please insert arguments. \n"));
		char parent[51],path[51],name[51];
		scanf("%50s %50s %50s", parent,path,name);
		_tprintf(TEXT("[Log _tmain] Start program. Argument are :Parent: %s\tPath: %s\tName: %s \n"), argv[1], argv[2], argv[3]);
	}
	else if (argc == 4) {
		_tprintf(TEXT("[Log _tmain] Start program. Arguments are : Parent: %s\tPath: %s\tName: %s \n"), argv[1], argv[2], argv[3]);
		root_hKey = openKey(argv[1], argv[2], argv[3]);
		beginCreateFiles(root_hKey, DIR);
	}
	else if (argc>4)
		_tprintf(TEXT("[Log _tmain] Start program. Taken arguments are : Parent: %s\t Path: %s\t Name: %s\t (other %d are ignored)\n"), argv[1], argv[2], argv[3], argc - 2);
	RegCloseKey(root_hKey);
}

///function that finds the corespondent key register handler for the HIVE
HKEY getParent(TCHAR parent_name[]) {
	if (_tcscmp(parent_name, "HKEY_CLASSES_ROOT")==0)
		return HKEY_CLASSES_ROOT;
	if (_tcscmp(parent_name, "HKEY_CURRENT_USER")==0)
		return HKEY_CURRENT_USER;
	if (_tcscmp(parent_name, "HKEY_LOCAL_MACHINE")==0)
		return HKEY_LOCAL_MACHINE;
	if (_tcscmp(parent_name, "HKEY_USERS")==0)
		return HKEY_USERS;
	if (_tcscmp(parent_name, "HKEY_CURRENT_CONFIG")==0)
		return HKEY_CURRENT_CONFIG;
	_tprintf(TEXT("[Log] Error: Cannot find parent key."));
}

///function that returns the type of the file
TCHAR* getFileType(DWORD type) {

	switch (type)
	{
	case 1:return "REG_SZ\r\n";
		break;
	case 2: return "REG_EXPAND_SZ\r\n";
		break;
	case 3:return "REG_BINARY\r\n";
		break;
	case 4:return "REG_DWORD\r\n";
		break;
	default:return "Other\r\n";
		break;
	}
}

///function that opens the key with name @key_name path @path and HIVE key name @parent_name and returns the handle to the named key register
HKEY openKey(TCHAR parent_name[], TCHAR path[], TCHAR key_name[])
{
	HKEY hKey = NULL;
	
    TCHAR *full_path = strdup(path);
	strcat(full_path, "\\");
	strcat(full_path, key_name);

	//open register key
	long sts_reg_key = RegOpenKeyEx(getParent(parent_name), _T(full_path), 0, KEY_READ, &hKey);
	//test register key
	if (ERROR_SUCCESS != sts_reg_key)
	{
		_tprintf(TEXT("[Log openKey] Error: Cannot open registry key : %s \tError: %d\n"), full_path, sts_reg_key);
	}
	else //if it exists, get value from it
	{
		_tprintf(TEXT("[Log openKey] Success: opened registry key : %s \t Parent %s \n"), full_path, parent_name);
	}

	//Return the current value of the key
	return hKey;
}

///function to concatenate 2 TCHARS for a path
TCHAR* concatenatePath(TCHAR* dir, TCHAR* key) {
	TCHAR * path = (TCHAR*)malloc(256);
	_tcscpy(path, dir);
	_tcscat(path, "\\");
	_tcscat(path, key);
	return path;
}

///function that follows recursively the key register tree and makes new files/folders according to the info
void beginCreateFiles(HKEY root_hKey, TCHAR* dir) {

	HKEY hKey;
	LONG stsRegKey, subkeyResult, fileResult;
	DWORD keysCount, valuesCount, subkeySize;
	TCHAR subkeyName[MAX_PATH];

	// get the class name and the value count. 
	stsRegKey = RegQueryInfoKey(root_hKey, 0, 0, 0, &keysCount, 0, 0, &valuesCount, 0, 0, 0, 0);
	_tprintf(TEXT("[Log beginCreateFiles] Create folder, keys nr= %d \t directory= %s \n", keysCount, dir));
	BOOLEAN dirSts=CreateDirectory(dir, NULL);

	if(!dirSts)
		_tprintf(TEXT("[Log beginCreateFiles] Error creating folder"));
	// enumerate the subkeys
	for (DWORD i = 0; i < keysCount; i++) {
		subkeySize = MAX_PATH;
		subkeyResult = RegEnumKeyEx(root_hKey, i, subkeyName, &subkeySize, NULL, NULL, NULL, NULL);
		_tprintf(TEXT("[Log beginCreateFiles] Subkey no %d : name= %s \t  size = %d \n", i, subkeyName, subkeySize));

		stsRegKey = RegOpenKeyEx(root_hKey, subkeyName, NULL, KEY_READ, &hKey);
		if (ERROR_SUCCESS != stsRegKey)
		{
			_tprintf(TEXT("[Log beginCreateFiles] Error: Cannot open registry key : %s \tError: %d\n"), subkeyName, stsRegKey);
		}
		else //if it exists, get value from it
		{
			_tprintf(TEXT("[Log beginCreateFiles] Success: opened registry key : %s \t  \n"), subkeyName);
		}

		TCHAR* newDir;
		newDir = concatenatePath(dir, subkeyName);
		//recursive call
		beginCreateFiles(hKey, newDir);
		
		RegCloseKey(hKey);
	}

	//
	if (valuesCount > 0) {
		_tprintf(TEXT("[Log beginCreateFiles] Create file, nr of values = %d \t directory= %s \n", valuesCount, dir));
		for (DWORD i = 0; i < valuesCount; i++) {
			BYTE data[MAX_PATH];
			DWORD type;
			DWORD dataSize = MAX_PATH;
			subkeySize = MAX_PATH;
			TCHAR fileName[MAX_PATH];

			fileResult = RegEnumValue(root_hKey, i, fileName, &subkeySize, NULL, &type, data, &dataSize);
			_tprintf(TEXT("[Log beginCreateFiles] Create file: name=%s = %d \t type= %d size=%d \n", fileName, type, dataSize));
			createFile(dir, fileName, data, dataSize, type);
		}
	}
}

///function creates a new file
void createFile(TCHAR* dir, TCHAR* name, BYTE* DataBuffer, DWORD dwBytesToWrite, DWORD type) {
		char buff[256];

		_tprintf(TEXT("[Log createFile] Create file: name=%s = %d \t type= %d size=%d \n", name, type, dwBytesToWrite));
		sprintf_s(buff, 256, TEXT("%s%s%s"), dir, name, ".txt");

		HANDLE hFile = CreateFile(buff, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{

			_tprintf(TEXT("[Log createFile] Terminal failure: Unable to open file for write.\n"));
			return;
		}

		DWORD dwBytesWritten, dwBytesToWriteOUT;
		TCHAR* dataType = getFileType(type);
		dwBytesToWriteOUT = strlen(dataType);
		BOOL writeFile= WriteFile(hFile, dataType, dwBytesToWriteOUT, &dwBytesWritten, NULL);
		writeFile = WriteFile(hFile, DataBuffer, dwBytesToWrite, &dwBytesWritten, NULL);

		if (FALSE == writeFile)
		{
			_tprintf(TEXT("[Log createFile] Terminal failure: Unable to write to file.\n"));
		}
		else {
			if (dwBytesWritten != dwBytesToWrite) {
				_tprintf(TEXT("[Log createFile] Error: dwBytesWritten != dwBytesToWrite\n"));
			}
			else {
				_tprintf(TEXT("[Log createFile] Wrote successfully.\n"));
	
			}
		}
		CloseHandle(hFile);
	}


