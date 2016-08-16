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
        printf("------���̵߳�%d�ν���ѭ��------\n", iIndex);
        Sleep(4);
        printf("------���̵߳�%d���뿪ѭ��------\n\n", iIndex);
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
        printf("------���̵߳�%d�ν���ѭ��------\n", iIndex);
        mainLoop();
        printf("------���̵߳�%d���뿪ѭ��------\n\n", iIndex);
        SetEvent(g_ThreadEvent);
    }
}
int main(int argc, char* argv[])
{
    //ȫΪ�Զ�ģʽһ��ֻ�ܽ�ȥһ��
    g_ThreadEvent = CreateEvent(NULL,  // default security attributes
                                false, // auto-reset event
                                true,  // initial state is signaled
                                NULL); // object name

    //����Ϊδ����
    g_MainEvent = CreateEvent(NULL, // default security attributes
                              false,// auto-reset event
                              false,// initial state is nonsignaled
                              NULL);// object name

    // ����Ϊ���ź�״̬
    ResetEvent(g_MainEvent);
    HANDLE hThread = CreateThread(NULL,       // default security attributes
                                  0,          // use default stack size
                                  ThreadLoop, // thread function name
                                  NULL,       // argument to thread function
                                  0,          // use default creation flags
                                  NULL);      // returns the thread identifier

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

