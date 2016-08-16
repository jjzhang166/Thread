// ThreadSync3.cpp : �������̨Ӧ�ó������ڵ㡣
//

/********************************************************
* ���ĸ��߳�1��2��3��4���߳�1�Ĺ��ܾ������1���߳�2�Ĺ��ܾ������2,
* �Դ�����,�������ĸ��ļ�ABCD,��ʼ��Ϊ�ա���Ҫ���ĸ��ļ������¸�ʽ:
* A��1 2 3 4 1 2....
* B��2 3 4 1 2 3....
* C��3 4 1 2 3 4....
* D��4 1 2 3 4 1....
*********************************************************/

#include "stdafx.h"

#include <windows.h>
#include <iostream>

using namespace std;

//���߳��¼�
HANDLE  g_ThreadEvent[3];
int g_EventIndex = 0;

DWORD WINAPI ThreadFunc(void *p)
{
    int param = (int)p;
    char c = 'A' + param;

    for (int iIndex = 0; iIndex < 1; iIndex++)
    {
        //�߳�ABC�ֱ�ȴ��¼�0,1,2
        WaitForSingleObject(g_ThreadEvent[param], INFINITE);
        printf("the thread is %c\n", c);
        ResetEvent(g_ThreadEvent[g_EventIndex]);
        g_EventIndex = (g_EventIndex + 1) % 3;
        SetEvent(g_ThreadEvent[g_EventIndex]);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    //ȫΪ�Զ�ģʽһ��ֻ�ܽ�ȥһ��
    for (int iIndex = 0; iIndex < 3; iIndex++)
    {
        g_ThreadEvent[iIndex] = CreateEvent(NULL, // default security attributes
                                            false,// auto-reset event
                                            false,// initial state is nonsignaled
                                            NULL);// object name
    }

    HANDLE hThread[3];

    for (int iRunIndex = 0; iRunIndex < 3; iRunIndex++)
    {
        g_EventIndex = iRunIndex;
        printf("\n��%d�����п�ʼ\n", iRunIndex);
        SetEvent(g_ThreadEvent[g_EventIndex]);

        for (int iIndex = 0; iIndex < 3; iIndex++)
        {
            hThread[iIndex] = CreateThread(NULL,           // default security attributes
                                           0,              // use default stack size
                                           ThreadFunc,     // thread function name
                                           (void *)iIndex, // argument to thread function
                                           0,              // use default creation flags
                                           NULL);          // returns the thread identifier
        }

        //�ȴ������߳̽���
        WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

        for (int iIndex = 0; iIndex < 3; iIndex++)
        {
            ResetEvent(g_ThreadEvent[iIndex]);
            CloseHandle(hThread[iIndex]);
        }

        printf("��%d�����н���\n", iRunIndex);
    }

    cout << "���н�������������˳�.....\n";

    char c = getchar();
    CloseHandle(g_ThreadEvent);

    return 0;
}

