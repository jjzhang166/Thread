// Part2Listing2.cpp : 定义控制台应用程序的入口点。
//
/*  file Main.cpp
*
*  This program is an adaptation of the code Rex Jaeschke showed in
*  Listing 3 of his Nov 2005 C/C++ User's Journal article entitled
*  "C++/CLI Threading: Part II".  I changed it from C++/CLI (managed)
*  code to standard C++.
*
*  One hassle is the fact that C++ must employ a free (C) function
*  or a static class member function as the thread entry function.
*
*  This program must be compiled with a multi-threaded C run-time
*  (/MT for LIBCMT.LIB or /MTd for LIBCMTD.LIB).
*
*                                      John Kopplin  7/2006
*/
#include "stdafx.h"

#include <stdio.h>
#include <windows.h>          // for HANDLE
#include <process.h>          // for _beginthread()
#include <string>             // for STL string class

using namespace std;


//#ifdef WITH_SYNCHRONIZATION

class ThreadX
{
private:

    int        m1;
    static int m2;
    static  __declspec(thread)  int m3;


#ifdef WITH_SYNCHRONIZATION
    CRITICAL_SECTION m_CriticalSection;
#endif

public:
    string threadName;

    ThreadX()         // ctor
    {
        m1 = 10;
#ifdef WITH_SYNCHRONIZATION
        InitializeCriticalSection(&m_CriticalSection);
#endif
    }

    virtual ~ThreadX()   // dtor
    {
#ifdef WITH_SYNCHRONIZATION
        // Release resources used by the critical section object.
        DeleteCriticalSection(&m_CriticalSection);
#endif
    }

    void TMain(void)
    {
        // A "synchronization object" is an object whose handle can
        // be specified in one of the wait functions to coordinate
        // the execution of multiple threads. More than one process
        // can have a handle to the same synchronization object,
        // making interprocess synchronization possible.

        // The synchronization objects provided by WIN32 are:
        //   event
        //   mutex or critical section
        //   semaphore
        //   waitable timer


#ifdef WITH_SYNCHRONIZATION
        EnterCriticalSection(&m_CriticalSection);   // Jaeschke's Monitor::Enter()
#endif

        for (int i = 1; i <= 5; i++)
        {
            ++m1;
            ++m2;
            ++m3;
        }

        printf("Thread %s: m1 = %d, m2 = %d, m3 = %d\n", threadName.c_str(), m1, m2, m3);

#ifdef WITH_SYNCHRONIZATION
        LeaveCriticalSection(&m_CriticalSection);   // Jaeschke's Monitor::Exit(this)
#endif

    }

    // In C++ you must employ a free (C) function or a
    // static class member function as the thread entry function.
    // Furthermore, _beginthreadex() demands that the thread
    // entry function signature take a single (void*) and returned
    // an unsigned.

    static unsigned __stdcall ThreadStaticTMain(void * pThis)
    {
        ThreadX * pthX = (ThreadX*)pThis;
        pthX->TMain();

        // A thread terminates automatically if it completes execution,
        // or it can terminate itself with a call to _endthread().

        return 1;          // the thread exit code
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


    ThreadX * o1 = new ThreadX();    // Jaeschke's gcnew ThreadX

    HANDLE   hth1;
    unsigned  uiThread1ID;

    hth1 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        ThreadX::ThreadStaticTMain,
        o1,           // arg list
        CREATE_SUSPENDED,  // so we can later call ResumeThread()
        &uiThread1ID);

    if (hth1 == 0)
        printf("Failed to create thread 1\n");

    DWORD   dwExitCode;

    GetExitCodeThread(hth1, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial thread 1 exit code = %u\n", dwExitCode);

    o1->threadName = "t1";

    ThreadX * o2 = new ThreadX();

    HANDLE   hth2;
    unsigned  uiThread2ID;

    hth2 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        ThreadX::ThreadStaticTMain,
        o2,           // arg list
        CREATE_SUSPENDED,  // so we can later call ResumeThread()
        &uiThread2ID);

    if (hth2 == 0)
        printf("Failed to create thread 2\n");

    GetExitCodeThread(hth2, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial thread 2 exit code = %u\n", dwExitCode);

    o2->threadName = "t2";

    // If we hadn't specified CREATE_SUSPENDED in the call to _beginthreadex()
    // we wouldn't now need to call ResumeThread().

    ResumeThread(hth1);   // Jaeschke's   // t1->Start();
    ResumeThread(hth2);   // Jaeschke's   // t2->Start();

                          // In C++ the process terminates when the primary thread exits
                          // and when the process terminates all its threads are then terminated.
                          // Hence if you comment out the following waits, the non-primary
                          // threads will never get a chance to run.

    WaitForSingleObject(hth1, INFINITE);  // Jaeschke's t1->Join()
    WaitForSingleObject(hth2, INFINITE);  // Jaeschke's t2->Join()

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
}


