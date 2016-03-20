// ThreadSync2.cpp : �������̨Ӧ�ó������ڵ㡣
//

/********************************************************
* ���߳�ѭ��10��,�������߳�ѭ��100��,�����ֻص����߳�ѭ��10��,
* �����ٻص����߳���ѭ��100��,���ѭ��50��
*********************************************************/

#include "stdafx.h"

#include <windows.h>
#include <iostream>

using namespace std;

//���߳��¼�
HANDLE g_ThreadEvent;

//���߳��¼�
HANDLE g_MainEvent;

DWORD WINAPI ThreadLoop(LPVOID lpParam)
{
    for (int iIndex = 0; iIndex < 50; iIndex++)
    {
        WaitForSingleObject(g_ThreadEvent, INFINITE);
        ResetEvent(g_MainEvent);
        printf("\n\t\t���߳̽���ѭ��\n");
        Sleep(4);
        printf("\n\t\t���߳��뿪ѭ��\n");
        SetEvent(g_MainEvent);
    }

    return 0;
}

void mainLoop()
{
    Sleep(5);
}

void mainRun()
{
    for (int iIndex = 0; iIndex < 50; iIndex++)
    {
        WaitForSingleObject(g_MainEvent, INFINITE);
        ResetEvent(g_ThreadEvent);
        printf("\n\t\t���߳̽���ѭ��\n");
        mainLoop();
        printf("\n\t\t���߳��뿪ѭ��\n");
        SetEvent(g_ThreadEvent);
    }
}
int main(int argc, char* argv[])
{
    //ȫΪ�Զ�ģʽһ��ֻ�ܽ�ȥһ��
    g_ThreadEvent = CreateEvent(NULL, false, true, NULL);

    //����Ϊδ����
    g_MainEvent = CreateEvent(NULL, false, false, NULL);

    ResetEvent(g_MainEvent);
    HANDLE hThread = (HANDLE)CreateThread(NULL, 0, ThreadLoop, NULL, 0, NULL);

    mainRun();
    //�ȴ������߳̽���
    WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
    cout << "���н�������������˳�.....\n";

    char c = getchar();
    CloseHandle(g_ThreadEvent);
    CloseHandle(g_MainEvent);
    CloseHandle(hThread);

    return 0;
}

