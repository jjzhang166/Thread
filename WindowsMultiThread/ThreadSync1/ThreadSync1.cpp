// ThreadSync1.cpp : �������̨Ӧ�ó������ڵ㡣
//
/********************************************************
* ��дһ������,����3���߳�,��3���̵߳�ID�ֱ�ΪA��B��C,
* ÿ���߳̽��Լ���ID����Ļ�ϴ�ӡ10��,Ҫ�����������밴ABC��˳����ʾ;
* ��:ABCABC��.���ε���
* ��Ȼ�Ƕ��߳�ͬ��������,A����,B��,B����C,����ѭ��,ʹ���¼������
*********************************************************/
#include "stdafx.h"

#include<windows.h>
#include<iostream>

using namespace std;

//���߳��¼�
HANDLE g_ThreadEvent[3];

int g_EventIndex = 0;

DWORD WINAPI ThreadFunc(void *p)
{
    int param = (int)p;
    char c = 'A' + param;

    for (int iIndex = 0; iIndex < 10; iIndex++)
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
    for (int iIndex = 0; iIndex < 3; ++iIndex)
    {
        g_ThreadEvent[iIndex] = CreateEvent(NULL, false, false, NULL);
    }

    SetEvent(g_ThreadEvent[0]);

    HANDLE hThread[3];

    for (int iIndex = 0; iIndex < 3; iIndex++)
    {
        hThread[iIndex] = (HANDLE)CreateThread(NULL, 0, ThreadFunc, (void *)iIndex, 0, NULL);
    }

    //�ȴ������߳̽���
    WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
    cout << "���н�������������˳�.....\n";

    char c = getchar();
    CloseHandle(g_ThreadEvent);

    return 0;
}

