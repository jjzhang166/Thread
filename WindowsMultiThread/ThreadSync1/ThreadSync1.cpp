// ThreadSync1.cpp : 定义控制台应用程序的入口点。
//
/********************************************************
* 编写一个程序,开启3个线程,这3个线程的ID分别为A、B、C,
* 每个线程将自己的ID在屏幕上打印10遍,要求输出结果必须按ABC的顺序显示;
* 如:ABCABC….依次递推
* 依然是多线程同步的问题,A做完,B做,B做完C,依次循环,使用事件来完成
*********************************************************/
#include "stdafx.h"

#include<windows.h>
#include<iostream>

using namespace std;

//子线程事件
HANDLE g_ThreadEvent[3];

int g_EventIndex = 0;

DWORD WINAPI ThreadFunc(void *p)
{
    int param = (int)p;
    char c = 'A' + param;

    for (int iIndex = 0; iIndex < 10; iIndex++)
    {
        //线程ABC分别等待事件0,1,2
        WaitForSingleObject(g_ThreadEvent[param], INFINITE);
        printf("the thread is %c\n", c);

        //重置为无信号状态
        ResetEvent(g_ThreadEvent[g_EventIndex]);
        g_EventIndex = (g_EventIndex + 1) % 3;

        // 将另外一个事件设置为有信号状态
        SetEvent(g_ThreadEvent[g_EventIndex]);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    //全为自动模式一次只能进去一个
    for (int iIndex = 0; iIndex < 3; ++iIndex)
    {
        g_ThreadEvent[iIndex] = CreateEvent(NULL,  // default security attributes
                                            false, // auto-reset event
                                            false, // initial state is nonsignaled
                                            NULL); // object name
    }

    SetEvent(g_ThreadEvent[0]);

    HANDLE hThread[3];

    for (int iIndex = 0; iIndex < 3; iIndex++)
    {
        hThread[iIndex] = CreateThread(NULL,          // default security attributes
                                       0,             // use default stack size 
                                       ThreadFunc,    // thread function name
                                       (void *)iIndex,// argument to thread function 
                                       0,             // use default creation flags 
                                       NULL);         // returns the thread identifier 
    }

    //等待所有线程结束
    WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
    cout << "运行结束，按任意键退出.....\n";

    char c = getchar();

    for (auto index = 0; index < 3; ++index) {
        CloseHandle(g_ThreadEvent[index]);
    }

    for (auto index = 0; index < 3; ++index){
        CloseHandle(hThread[index]);
    }

    return 0;
}

