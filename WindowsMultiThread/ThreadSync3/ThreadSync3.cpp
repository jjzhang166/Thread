// ThreadSync3.cpp : 定义控制台应用程序的入口点。
//

/********************************************************
* 有四个线程1、2、3、4。线程1的功能就是输出1，线程2的功能就是输出2,
* 以此类推,现在有四个文件ABCD,初始都为空。现要让四个文件呈如下格式:
* A：1 2 3 4 1 2....
* B：2 3 4 1 2 3....
* C：3 4 1 2 3 4....
* D：4 1 2 3 4 1....
*********************************************************/

#include "stdafx.h"

#include <windows.h>
#include <iostream>

using namespace std;

//子线程事件
HANDLE  g_ThreadEvent[3];
int g_EventIndex = 0;

DWORD WINAPI ThreadFunc(void *p)
{
    int param = (int)p;
    char c = 'A' + param;

    for (int iIndex = 0; iIndex < 1; iIndex++)
    {
        //线程ABC分别等待事件0,1,2
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
    //全为自动模式一次只能进去一个
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
        printf("\n第%d次运行开始\n", iRunIndex);
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

        //等待所有线程结束
        WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

        for (int iIndex = 0; iIndex < 3; iIndex++)
        {
            ResetEvent(g_ThreadEvent[iIndex]);
            CloseHandle(hThread[iIndex]);
        }

        printf("第%d次运行结束\n", iRunIndex);
    }

    cout << "运行结束，按任意键退出.....\n";

    char c = getchar();
    CloseHandle(g_ThreadEvent);

    return 0;
}

