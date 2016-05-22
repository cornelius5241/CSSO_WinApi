#include <stdlib.h>    
#include <windows.h>
#include "stdafx.h"
#include<wininet.h>

const int _SIZE = 307200;

int _tmain(int argc, _TCHAR* argv[])
{
	HINTERNET hIOPen;
	HINTERNET hIOpenUrl;
	HINTERNET hHttpOpenReq;

	LPCTSTR user_agent = (LPCTSTR)"CSSO_WH_4";
	LPCTSTR url = (LPCTSTR)"students.info.uaic.ro";
	LPCTSTR verb = (LPCTSTR)"GET";
	LPCTSTR object = (LPCTSTR)"/~corneliu.tutuianu/info.txt";
	LPCTSTR acceptTypes[2] = { (LPCTSTR)"*/*",NULL };

	char* buffer=new char[_SIZE];
	DWORD size;

	char *ip;
	char *token = NULL;
	char *filePath;
	char *userName;
	char *password;

	hIOPen = InternetOpen(user_agent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (!hIOPen) {
		_tprintf("Error InternetOpen! \n",GetLastError());
		return(FALSE);
	}

	hIOpenUrl = InternetConnect(hIOPen, url, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
	if (!hIOpenUrl) {
		_tprintf("Error InternetConnect! \n",GetLastError());
		return(FALSE);
	}

	hHttpOpenReq = HttpOpenRequest(hIOpenUrl, verb, object, 0, 0, acceptTypes, INTERNET_FLAG_RELOAD, 0);
	if (!hHttpOpenReq) {
		_tprintf("Error HttpOpenRequest! \n",GetLastError());
		return(FALSE);
	}

	if (!HttpSendRequest(hHttpOpenReq, NULL, NULL, NULL, NULL))
	{
		_tprintf("Error HttpSendRequest! \n",GetLastError());
		return(FALSE);
	}

	if (!InternetReadFile(hHttpOpenReq, buffer, _SIZE, &size))
	{
		_tprintf("Error InternetReadFile! \n",GetLastError());
		return(FALSE);
	}

	ip = strtok_s(buffer, "\n", &token);
	ip[strlen(ip)-1] = '\0';
	_tprintf("%s \n", ip);

	filePath = strtok_s(NULL, "\n", &token);
	filePath[strlen(filePath) - 1] = '\0';
	_tprintf("%s\n", filePath);

	userName = strtok_s(NULL, "\n", &token);
	userName[strlen(userName) - 1] = '\0';
	_tprintf("%s\n", userName);

	password = strtok_s(NULL, "\n", &token);
	password[strlen(password)-1 ] = '\0';
	_tprintf("%s\n", "******");

	HINTERNET hFtp;
	hFtp = InternetConnect(hIOPen, ip, INTERNET_DEFAULT_FTP_PORT, userName, password, INTERNET_SERVICE_FTP, NULL, NULL);
	if (hFtp == NULL)
	{
		_tprintf("Error InternetConnect ftp! %ld\n", GetLastError());
		return(FALSE);
	}

	if (!FtpPutFile(hFtp, filePath, "/calc.exe", FTP_TRANSFER_TYPE_BINARY, 0))
	{
		_tprintf("Error FtpPutFile ftp! %ld\n", GetLastError());
		return(FALSE);
	}

	InternetCloseHandle(hFtp);
	InternetCloseHandle(hHttpOpenReq);
	InternetCloseHandle(hIOpenUrl);
	InternetCloseHandle(hIOPen);
}
