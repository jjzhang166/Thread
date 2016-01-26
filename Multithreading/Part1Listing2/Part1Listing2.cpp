// Part1Listing2.cpp : 定义控制台应用程序的入口点。
//
/*  file Main.cpp
*
*  This program is an adaptation of the code Rex Jaeschke showed in
*  Listing 2 of his Oct 2005 C/C++ User's Journal article entitled
*  "C++/CLI Threading: Part I".  I changed it from C++/CLI (managed)
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

//#define WITH_SYNCHRONIZATION    // undef to see the problem

class Point
{
private:
    int x;
    int y;

#ifdef WITH_SYNCHRONIZATION
    CRITICAL_SECTION m_CriticalSection;
#endif

public:

    Point()
    {
#ifdef WITH_SYNCHRONIZATION
        InitializeCriticalSection(&m_CriticalSection);
#endif
    }

    virtual ~Point()
    {
#ifdef WITH_SYNCHRONIZATION
        // Release resources used by the critical section object.
        DeleteCriticalSection(&m_CriticalSection);
#endif
    }

    // Jaeschke defines an X and Y property here.  Straight
    // C++ doesn't offer "properties".

    //  property int X
    //  {
    //    int get() { return x; }
    //    void set(int val) { x = val; }
    //  }

    //  property int Y
    //  {
    //    int get() { return y; }
    //    void set(int val) { y = val; }
    //  }


    void Move(int xor, int yor)
    {
        // A "synchronization object" is an object whose handle can
        // be specified in one of the wait functions to coordinate
        // the execution of multiple threads. More than one process
        // can have a handle to the same synchronization object,
        // making interprocess synchronization possible.

        // The synchronization objects provided by WIN32 are:
        //   event
        //   mutex
        //   semaphore
        //   waitable timer

        // An event notifies one or more waiting threads that an event
        // has occurred.

        // A mutex can be owned by only one thread at a time, enabling
        // threads to coordinate mutually exclusive access to a shared resource.
        // "Critical section" objects provide synchronization similar to
        // that provided by mutex objects, except that critical section
        // objects can be used only by the threads of a single process
        // (hence they are lighter weight than a mutex). Like a mutex object,
        // a critical section object can be owned by only one thread at a
        // time, which makes it useful for protecting a shared resource
        // from simultaneous access. There is no guarantee about the order
        // in which threads will obtain ownership of the critical section,
        // however, the operating system will be fair to all threads.
        // Another difference between a mutex and a critical section is
        // that if the critical section object is currently owned by another
        // thread, EnterCriticalSection() waits indefinitely for ownership
        // whereas WaitForSingleObject(), which is used with a mutex, allows
        // you to specify a timeout.

        // A semaphore maintains a count between zero and some maximum value,
        // limiting the number of threads that are simultaneously accessing
        // a shared resource.

        // A waitable timer notifies one or more waiting threads that a
        // specified time has arrived.

#ifdef WITH_SYNCHRONIZATION
        EnterCriticalSection(&m_CriticalSection);   // Jaeschke's Monitor::Enter(this)
#endif

        x = xor;   // Jaeschke's code used capital X here, invoking the property
        y = yor;

#ifdef WITH_SYNCHRONIZATION
        LeaveCriticalSection(&m_CriticalSection);   // Jaeschke's Monitor::Exit(this)
#endif

    }

    char * ToString()
    {
        int xCopy;
        int yCopy;

#ifdef WITH_SYNCHRONIZATION
        EnterCriticalSection(&m_CriticalSection);
#endif

        xCopy = x;
        yCopy = y;

#ifdef WITH_SYNCHRONIZATION
        LeaveCriticalSection(&m_CriticalSection);
#endif

        static char   szTemp[128];

        if (xCopy == yCopy)
            sprintf_s(szTemp, "(%d,%d)", xCopy, yCopy);
        else
            sprintf_s(szTemp, "(%d,%d) ERROR", xCopy, yCopy);

        return szTemp;
    }

};

class ThreadY
{
private:
    Point* pnt;
    bool   mover;
public:
    ThreadY(bool isMover, Point * p)
    {
        mover = isMover;
        pnt = p;
    }

    // In C++ you must employ a free (C) function or a
    // static class member function as the thread entry function.
    // Furthermore, _beginthreadex() demands that the thread
    // entry function signature take a single (void*) and returned
    // an unsigned.

    static unsigned __stdcall ThreadStaticStartUp(void * pThis)
    {
        ThreadY * pthY = (ThreadY*)pThis;
        pthY->StartUp();

        // A thread terminates automatically if it completes execution,
        // or it can terminate itself with a call to _endthread().

        return 1;          // the thread exit code
    }

    void StartUp()
    {
        if (mover)
        {
            for (int i = 1; i <= 200000000; ++i)
            {
                pnt->Move(i, i);
            }
        }
        else
        {
            for (int i = 1; i <= 200; ++i)
            {
                printf("%s\n", pnt->ToString());

                Sleep(1);
            }
        }
    }
};

int main()
{
    // In this program we create 2 threads which each access the
    // same Point object.  Hence we will need to provide synchronization.
    // Because in one case we set isMover = TRUE and in the other case
    // we set isMover = FALSE, one of the threads continually changes
    // the Point's x and y values while the other thread displays these
    // values. Note that both threads have the same thread entry function
    // even though they act completely differently.

    // With the synchronization statements removed, you will have to
    // look closely at the program output to spot those locations where
    // x and y differ.  They will differ by only 1, with x being the
    // larger number.

    Point * p = new Point();

    ThreadY * o1 = new ThreadY(true, p);

    HANDLE   hth1;
    unsigned  uiThread1ID;

    hth1 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        ThreadY::ThreadStaticStartUp,
        o1,           // arg list
        CREATE_SUSPENDED,  // so we can later call ResumeThread()
        &uiThread1ID);

    if (hth1 == 0)
        printf("Failed to create thread 1\n");

    DWORD   dwExitCode;

    GetExitCodeThread(hth1, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial thread 1 exit code = %u\n", dwExitCode);

    ThreadY * o2 = new ThreadY(false, p);

    HANDLE   hth2;
    unsigned  uiThread2ID;

    hth2 = (HANDLE)_beginthreadex(NULL,         // security
        0,            // stack size
        ThreadY::ThreadStaticStartUp,
        o2,           // arg list
        CREATE_SUSPENDED,  // so we can later call ResumeThread()
        &uiThread2ID);

    if (hth2 == 0)
        printf("Failed to create thread 2\n");

    GetExitCodeThread(hth2, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial thread 2 exit code = %u\n", dwExitCode);

    // If we hadn't specified CREATE_SUSPENDED in the call to _beginthreadex()
    // we wouldn't now need to call ResumeThread().

    ResumeThread(hth1);   // Jaeschke's   // t1->Start();
    ResumeThread(hth2);

    // We now want the primary thread to sleep to allow time for
    // the other threads to come alive.  This ensures the primary
    // thread will have to compete for access to the Point object.

    Sleep(100);


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


