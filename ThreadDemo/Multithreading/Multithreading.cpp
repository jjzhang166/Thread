// Multithreading.cpp : 定义控制台应用程序的入口点。
//

// http://stackoverflow.com/questions/12363071/critial-section-usage-in-multithreading

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <string>

using namespace std;

#define WITH_SYNCHRONIZATION

class ThreadX
{
private:
    int        m1;
    static int m2;                       // shared variable
    static  __declspec(thread)  int m3;  // thread local variable


#ifdef WITH_SYNCHRONIZATION
    CRITICAL_SECTION m_CriticalSection;
#endif

public:
    string threadName;

    ThreadX()
    {
        m1 = 10;
#ifdef WITH_SYNCHRONIZATION
        InitializeCriticalSection(&m_CriticalSection);
#endif
    }

    virtual ~ThreadX()
    {
#ifdef WITH_SYNCHRONIZATION
        // Release resources used by the critical section object.
        DeleteCriticalSection(&m_CriticalSection);
#endif
    }

    void TMain(void)
    {
#ifdef WITH_SYNCHRONIZATION
        EnterCriticalSection(&m_CriticalSection);
#endif

        for (int i = 1; i <= 50000; i++)
        {
            ++m1;  // init value 10
            ++m2;  // init value 20
            ++m3;  // init value 30
        }

        printf("Thread %s: m1 = %d, m2 = %d, m3 = %d\n", threadName.c_str(), m1, m2, m3);

#ifdef WITH_SYNCHRONIZATION
        LeaveCriticalSection(&m_CriticalSection);
#endif

    }

    static unsigned __stdcall ThreadStaticTMain(void * pThis)
    {
        ThreadX * pthX = (ThreadX*)pThis;
        pthX->TMain();

        return 1;
    }

};

int ThreadX::m2 = 20;
int ThreadX::m3 = 30;

int main()
{
    // In this program we create 2 threads and request that their
    // entry-point-function be the TMain() function of the ThreadX
    // class.  Because _beginthreadex() cannot accept a class member
    // function we must employ a 2 step process involving a tricky
    // cast to accomplish this.

    ThreadX * o1 = new ThreadX();

    HANDLE   hth1;
    unsigned  uiThread1ID;

    hth1 = (HANDLE)_beginthreadex(NULL,
                                  0,
                                  ThreadX::ThreadStaticTMain,
                                  o1,
                                  CREATE_SUSPENDED,
                                  &uiThread1ID);

    if (hth1 == 0)
    {
        printf("Failed to create thread 1\n");
    }

    DWORD   dwExitCode;

    GetExitCodeThread(hth1, &dwExitCode);
    printf("initial thread 1 exit code = %u\n", dwExitCode);

    o1->threadName = "t1";

    ThreadX * o2 = new ThreadX();

    HANDLE   hth2;
    unsigned  uiThread2ID;

    hth2 = (HANDLE)_beginthreadex(NULL,
                                  0,
                                  ThreadX::ThreadStaticTMain,
                                  o2,
                                  CREATE_SUSPENDED,
                                  &uiThread2ID);

    if (hth2 == 0)
    {
        printf("Failed to create thread 2\n");
    }

    GetExitCodeThread(hth2, &dwExitCode);
    printf("initial thread 2 exit code = %u\n", dwExitCode);

    o2->threadName = "t2";

    ResumeThread(hth1);
    ResumeThread(hth2);

    WaitForSingleObject(hth1, INFINITE);
    WaitForSingleObject(hth2, INFINITE);

    GetExitCodeThread(hth1, &dwExitCode);
    printf("thread 1 exited with code %u\n", dwExitCode);

    GetExitCodeThread(hth2, &dwExitCode);
    printf("thread 2 exited with code %u\n", dwExitCode);

    // The handle returned by _beginthreadex() has to be closed
    // by the caller of _beginthreadex().

    CloseHandle(hth1);
    CloseHandle(hth2);

    delete o1;
    o1 = NULL;

    delete o2;
    o2 = NULL;

    printf("Primary thread terminating.\n");
    return 0;
}

