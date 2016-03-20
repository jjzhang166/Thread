// ThreadSync2.cpp : 定义控制台应用程序的入口点。
//

/********************************************************
* 子线程循环10次,接着主线程循环100次,接着又回到子线程循环10次,
* 接着再回到主线程又循环100次,如此循环50次
*********************************************************/

#include "stdafx.h"

#include <windows.h>
#include <iostream>

using namespace std;

//子线程事件
HANDLE g_ThreadEvent;

//主线程事件
HANDLE g_MainEvent;

DWORD WINAPI ThreadLoop(LPVOID lpParam)
{
    for (int iIndex = 0; iIndex < 50; iIndex++)
    {
        WaitForSingleObject(g_ThreadEvent, INFINITE);
        ResetEvent(g_MainEvent);
        printf("\n\t\t子线程进入循环\n");
        Sleep(4);
        printf("\n\t\t子线程离开循环\n");
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
        printf("\n\t\t主线程进入循环\n");
        mainLoop();
        printf("\n\t\t主线程离开循环\n");
        SetEvent(g_ThreadEvent);
    }
}
int main(int argc, char* argv[])
{
    //全为自动模式一次只能进去一个
    g_ThreadEvent = CreateEvent(NULL, false, true, NULL);

    //设置为未触发
    g_MainEvent = CreateEvent(NULL, false, false, NULL);

    ResetEvent(g_MainEvent);
    HANDLE hThread = (HANDLE)CreateThread(NULL, 0, ThreadLoop, NULL, 0, NULL);

    mainRun();
    //等待所有线程结束
    WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
    cout << "运行结束，按任意键退出.....\n";

    char c = getchar();
    CloseHandle(g_ThreadEvent);
    CloseHandle(g_MainEvent);
    CloseHandle(hThread);

    return 0;
}

