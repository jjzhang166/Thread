// Part1Listing1.cpp : 定义控制台应用程序的入口点。
//
/*  file Main.cpp
*
*  This program is an adaptation of the code Rex Jaeschke showed in
*  Listing 1 of his Oct 2005 C/C++ User's Journal article entitled
*  "C++/CLI Threading: Part I".  I changed it from C++/CLI (managed)
*  code to standard C++.
*
*  One hassle is the fact that C++ must employ a free (C) function
*  or a static class member function as the thread entry function.
*
*  This program must be compiled with a multi-threaded C run-time
*  (/MT for LIBCMT.LIB in a release build or /MTd for LIBCMTD.LIB
*  in a debug build).
*
*                                      John Kopplin  7/2006
*/
#include "stdafx.h"

#include <stdio.h>
#include <string>             // for STL string class
#include <windows.h>          // for HANDLE
#include <process.h>          // for _beginthread()

using namespace std;


class ThreadX
{
private:
    int loopStart;
    int loopEnd;
    int dispFrequency;

public:
    string threadName;

    ThreadX(int startValue, int endValue, int frequency)
    {
        loopStart = startValue;
        loopEnd = endValue;
        dispFrequency = frequency;
    }

    // In C++ you must employ a free (C) function or a static
    // class member function as the thread entry-point-function.
    // Furthermore, _beginthreadex() demands that the thread
    // entry function signature take a single (void*) and returned
    // an unsigned.
    static unsigned __stdcall ThreadStaticEntryPoint(void * pThis)
    {
        ThreadX * pthX = (ThreadX*)pThis;   // the tricky cast
        pthX->ThreadEntryPoint();           // now call the true entry-point-function

                                            // A thread terminates automatically if it completes execution,
                                            // or it can terminate itself with a call to _endthread().

        return 1;          // the thread exit code
    }

    void ThreadEntryPoint()
    {
        // This is the desired entry-point-function but to get
        // here we have to use a 2 step procedure involving
        // the ThreadStaticEntryPoint() function.

        for (int i = loopStart; i <= loopEnd; ++i)
        {
            if (i % dispFrequency == 0)
            {
                printf("%s: i = %d\n", threadName.c_str(), i);
            }
        }
        printf("%s thread terminating\n", threadName.c_str());
    }
};


int main()
{
    // All processes get a primary thread automatically. This primary
    // thread can generate additional threads.  In this program the
    // primary thread creates 2 additional threads and all 3 threads
    // then run simultaneously without any synchronization.  No data
    // is shared between the threads.

    // We instantiate an object of the ThreadX class. Next we will
    // create a thread and specify that the thread is to begin executing
    // the function ThreadEntryPoint() on object o1. Once started,
    // this thread will execute until that function terminates or
    // until the overall process terminates.

    ThreadX * o1 = new ThreadX(0, 1000000, 20000);

    // When developing a multithreaded WIN32-based application with
    // Visual C++, you need to use the CRT thread functions to create
    // any threads that call CRT functions. Hence to create and terminate
    // threads, use _beginthreadex() and _endthreadex() instead of
    // the Win32 APIs CreateThread() and EndThread().

    // The multithread library LIBCMT.LIB includes the _beginthread()
    // and _endthread() functions. The _beginthread() function performs
    // initialization without which many C run-time functions will fail.
    // You must use _beginthread() instead of CreateThread() in C programs
    // built with LIBCMT.LIB if you intend to call C run-time functions.

    // Unlike the thread handle returned by _beginthread(), the thread handle
    // returned by _beginthreadex() can be used with the synchronization APIs.

    HANDLE   hth1;
    unsigned  uiThread1ID;

    hth1 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        ThreadX::ThreadStaticEntryPoint,
        o1,           // arg list
        CREATE_SUSPENDED,  // so we can later call ResumeThread()
        &uiThread1ID);

    if (hth1 == 0)
        printf("Failed to create thread 1\n");

    DWORD   dwExitCode;

    GetExitCodeThread(hth1, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial thread 1 exit code = %u\n", dwExitCode);

    // The System::Threading::Thread object in C++/CLI has a "Name" property.
    // To create the equivalent functionality in C++ I added a public data member
    // named threadName.

    o1->threadName = "t1";

    ThreadX * o2 = new ThreadX(-1000000, 0, 20000);

    HANDLE   hth2;
    unsigned  uiThread2ID;

    hth2 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        ThreadX::ThreadStaticEntryPoint,
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

    ResumeThread(hth1);   // serves the purpose of Jaeschke's t1->Start()

    ResumeThread(hth2);

    // In C++/CLI the process continues until the last thread exits.
    // That is, the thread's have independent lifetimes. Hence
    // Jaeschke's original code was designed to show that the primary
    // thread could exit and not influence the other threads.

    // However in C++ the process terminates when the primary thread exits
    // and when the process terminates all its threads are then terminated.
    // Hence if you comment out the following waits, the non-primary
    // threads will never get a chance to run.

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
}

