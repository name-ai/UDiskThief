//ʹ��ftp�ϴ�

#include "stdafx.h"
#include <time.h>
#include <WinInet.h>
#include "Shlwapi.h"

#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Shlwapi.lib")

#define FTP_ADDRESS TEXT("127.0.0.1")
//��ַ
#define FTP_USERNAME TEXT("aaaaaaaa")
//�û���
#define FTP_PASSWORD TEXT("aaaaaaa")
//����

//fileΪ�����ļ���·��
BOOL Upload2Ftp(TCHAR *file)
{
	TCHAR remoteFile[MAX_PATH] = TEXT("\0");
	_tcscat_s(remoteFile, PathFindFileName(file));

    HINTERNET hInternet;
    HINTERNET hFtpSession;
	HINTERNET hFtpFile;
    hInternet = InternetOpen(NULL,
		INTERNET_OPEN_TYPE_DIRECT,
		NULL,
		NULL,
		0);
	if(hInternet == NULL)
	{
		return FALSE;
	}
    hFtpSession = InternetConnect(hInternet, 
		FTP_ADDRESS, //��ַ
		INTERNET_DEFAULT_FTP_PORT, 
		FTP_USERNAME, //ftp�û���
		FTP_PASSWORD, //ftp����
		INTERNET_SERVICE_FTP, 
		0, 
		0);
	if(hFtpSession == NULL)
	{
		InternetCloseHandle(hInternet);
		return FALSE;
	}
	hFtpFile = FtpOpenFile(hFtpSession,
		remoteFile,
		GENERIC_WRITE,
		FTP_TRANSFER_TYPE_BINARY,
		0
		);
	if(hFtpFile == NULL)
	{
		InternetCloseHandle(hInternet);
		return FALSE;
	}

	//��ȡ�ļ��ϴ�
	char fileBuffer[MAX_PATH];
	DWORD sizeFile = MAX_PATH, sizeRemote = MAX_PATH;
	HANDLE hFile;
	hFile =  CreateFile(file,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	while(TRUE)
	{
		ZeroMemory(fileBuffer, MAX_PATH);
		if(ReadFile(hFile, fileBuffer,sizeFile, &sizeFile, NULL) == FALSE)
		{
			return FALSE;
		}
		if(sizeFile)
		{
			if(InternetWriteFile(hFtpFile, fileBuffer, sizeRemote, &sizeRemote) == FALSE)
			{
				return FALSE;
			}
		}
		else
		{
			break;
		}
	}
	
	CloseHandle(hFile);//�رձ����ļ�
    InternetCloseHandle(hFtpSession);
    InternetCloseHandle(hInternet);
	Sleep(2000);
//	MessageBox(NULL, file, TEXT("succeed"), MB_OK);
	
    return 0;
}


DWORD WINAPI uploadThread(LPVOID lpParam)
{
	TCHAR *file = (TCHAR*)lpParam;
	if(Upload2Ftp(file) == FALSE)
	{
		ExitThread(GetLastError());//�̳߳���ͷ��ش�����
	}
	return 0;
}

//ͨ�����߳��ϴ��ļ�
//fileΪ�ļ���·��
BOOL UploadViaNewThread(TCHAR *file)
{
	HANDLE hThread;
	hThread = CreateThread(NULL, 0, uploadThread, (LPVOID)file, 0, NULL);
	if(hThread == FALSE)
	{
		return FALSE;
	}
}