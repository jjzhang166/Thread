// Part2Listing3.cpp : 定义控制台应用程序的入口点。
//
/*  file Main.cpp
*
*  This program is an adaptation of the code Rex Jaeschke showed in
*  Listing 4 of his Nov 2005 C/C++ User's Journal article entitled
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

static bool interlocked = false;    // change this to fix the problem

const int maxCount = 100000000;
static LONG value = 0;              // under Windows Server 2003 you
                                    // could use LONGLONG here

unsigned __stdcall TMain(void* arg)
{
    if (interlocked)
    {
        for (int i = 1; i <= maxCount; i++)
        {
            InterlockedIncrement(&value); // under Windows Server 2003 you
                                          // could use InterlockedIncrement64() here
        }
    }
    else
    {
        for (int i = 1; i <= maxCount; i++)
        {
            ++value;
        }
    }

    return 3;  // thread exit code
}


int main()
{
    // In this program we create 3 threads and request that their
    // entry-point-function be the TMain() function which is a
    // free (C) function and hence causes no problems for
    // _beginthreadex()


    HANDLE   hth1;
    unsigned  uiThread1ID;

    hth1 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        TMain,        // entry-point-function
        NULL,         // arg list
        CREATE_SUSPENDED,  // so we can later call ResumeThread()
        &uiThread1ID);

    if (hth1 == 0)
        printf("Failed to create thread 1\n");

    DWORD   dwExitCode;

    GetExitCodeThread(hth1, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial thread 1 exit code = %u\n", dwExitCode);

    HANDLE   hth2;
    unsigned  uiThread2ID;

    hth2 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        TMain,        // entry-point-function
        NULL,         // arg list
        CREATE_SUSPENDED,  // so we can later call ResumeThread()
        &uiThread2ID);

    if (hth2 == 0)
        printf("Failed to create thread 2\n");

    GetExitCodeThread(hth2, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial thread 2 exit code = %u\n", dwExitCode);

    HANDLE   hth3;
    unsigned  uiThread3ID;

    hth3 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        TMain,        // entry-point-function
        NULL,         // arg list
        CREATE_SUSPENDED,  // so we can later call ResumeThread()
        &uiThread3ID);

    if (hth3 == 0)
        printf("Failed to create thread 3\n");

    GetExitCodeThread(hth3, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial thread 3 exit code = %u\n", dwExitCode);

    // If we hadn't specified CREATE_SUSPENDED in the call to _beginthreadex()
    // we wouldn't now need to call ResumeThread().

    ResumeThread(hth1);   // Jaeschke's   // t1->Start();
    ResumeThread(hth2);   // Jaeschke's   // t2->Start();
    ResumeThread(hth3);   // Jaeschke's   // t3->Start();

                          // In C++ the process terminates when the primary thread exits
                          // and when the process terminates all its threads are then terminated.
                          // Hence if you comment out the following waits, the non-primary
                          // threads will never get a chance to run.

    WaitForSingleObject(hth1, INFINITE);  // Jaeschke's t1->Join()
    WaitForSingleObject(hth2, INFINITE);  // Jaeschke's t2->Join()
    WaitForSingleObject(hth3, INFINITE);  // Jaeschke's t3->Join()

    GetExitCodeThread(hth1, &dwExitCode);
    printf("thread 1 exited with code %u\n", dwExitCode);

    GetExitCodeThread(hth2, &dwExitCode);
    printf("thread 2 exited with code %u\n", dwExitCode);

    GetExitCodeThread(hth3, &dwExitCode);
    printf("thread 3 exited with code %u\n", dwExitCode);

    printf("After %d operations, value = %d\n", 3 * maxCount, value);
    // under Windows Server 2003 you
    // could use %I64d

    // The handle returned by _beginthreadex() has to be closed
    // by the caller of _beginthreadex().

    CloseHandle(hth1);
    CloseHandle(hth2);
    CloseHandle(hth3);

    printf("Primary thread terminating.\n");
}


