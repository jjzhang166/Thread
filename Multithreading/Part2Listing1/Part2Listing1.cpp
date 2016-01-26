// Part2Listing1.cpp : 定义控制台应用程序的入口点。
//
/*  file Main.cpp
*
*  This program is an adaptation of the code Rex Jaeschke showed in
*  his Nov 2005 C/C++ User's Journal article entitled "C++/CLI Threading:
*  Part II".  I changed it from C++/CLI (managed) code to standard
*  C++.
*
*  One hassle is the fact that C++ must employ a free (C) function
*  or a static class member function as the thread entry-point-function.
*
*  Note that the Producer and Consumer share an instance of the
*  MessageBuffer class.  Hence the functions in this class must
*  be protected with a synchronization method.  I demonstrate the
*  use of a mutex here (see my Part1Listing2 program for a demonstration
*  of a critical section).
*
*  This program must be compiled with a multi-threaded C run-time
*  (/MT for LIBCMT.LIB or /MTd for LIBCMTD.LIB).
*
*                                      John Kopplin  3/2006
*/
#include "stdafx.h"

#include <stdio.h>
#include <windows.h>          // for HANDLE
#include <process.h>          // for _beginthread()

#define MSG_BUF_SIZE  128

//#define  WITH_SYNCHRONIZATION


class MessageBuffer
{
    // The functions in this MessageBuffer class must be synchronized
    // because they are called by both the producer and consumer threads.
    // Only one of these two threads must be allowed access to the
    // message buffer at any one time.

    // A "synchronization object" is an object whose handle can be
    // specified in one of the wait functions to coordinate the
    // execution of multiple threads. More than one process can have
    // a handle to the same synchronization object, making interprocess
    // synchronization possible.

    // A "mutex object" is a synchronization object whose state is set
    // to signaled when it is not owned by any thread, and nonsignaled
    // when it is owned. Only one thread at a time can own a mutex object,
    // whose name comes from the fact that it is useful in coordinating
    // mutually exclusive access to a shared resource.

    char  messageText[MSG_BUF_SIZE];  // the message buffer
    BOOL  m_bContinueProcessing;      // used to control thread lifetime

#ifdef WITH_SYNCHRONIZATION
    HANDLE  m_hMutex;
    HANDLE  m_hEvent;
#endif

public:
    MessageBuffer()            // ctor
    {
        memset(messageText, '\0', sizeof(messageText));  // initially the message buffer is empty

        m_bContinueProcessing = true;   // will be changed upon a call to DieDieDie()

#ifdef WITH_SYNCHRONIZATION
        printf("Creating mutex in MessageBuffer ctor.\n");

        m_hMutex = CreateMutex(NULL,    // no security attributes
            false,   // BOOL bInitialOwner, we don't want the
                     // thread that creates the mutex to
                     // immediately own it.
            "MessageBufferMutex" // lpName
            );
        if (m_hMutex == NULL)
        {
            printf("CreateMutex() failed in MessageBuffer ctor.\n");
        }

        // Create the auto-reset event.

        m_hEvent = CreateEvent(NULL,     // no security attributes
            FALSE,    // auto-reset event
            FALSE,    // initial state is non-signaled
            "MessageBufferEvent");    // lpName

        if (m_hEvent == NULL)
        {
            printf("CreateEvent() failed in MessageBuffer ctor.\n");
        }
#endif
    }

    ~MessageBuffer()
    {
#ifdef WITH_SYNCHRONIZATION
        CloseHandle(m_hMutex);
        CloseHandle(m_hEvent);
#endif
    }

    void SetMessage(char * s)
    {
        printf("in MessageBuffer::SetMessage()\n");

#ifdef WITH_SYNCHRONIZATION
        DWORD   dwWaitResult = WaitForSingleObject(m_hMutex, INFINITE); // Jaeschke's Monitor::Enter(this)

        if (dwWaitResult != WAIT_OBJECT_0)
        {
            printf("WaitForSingleObject() failed in MessageBuffer::SetMessage().\n");
            return;
        }
        printf("SetMessage() acquired mutex\n");
#endif

        // I intentionally use a very non-atomic method of copying
        // the new message into the message buffer, in order to
        // exacerbate the problem which occurs if the program doesn't
        // use a synchronization object between the producer and consumer.

        if (strlen(s) >= MSG_BUF_SIZE)
            s[MSG_BUF_SIZE - 1] = '\0';    // make sure the caller doesn't overfill our buffer

        char * pch = &messageText[0];
        while (*s)
        {
            *pch++ = *s++;
            Sleep(5);
        }

        *pch = '\0';

        // Since the message buffer now holds a message we can
        // allow the consumer thread to run.

        //        printf("Set new message: %s.\n", messageText );

#ifdef WITH_SYNCHRONIZATION
        printf("SetMessage() pulsing Event\n");
        if (!SetEvent(m_hEvent))   // Jaeschke's Monitor::Pulse(this)
        {
            printf("SetEvent() failed in SetMessage()\n");
        }

        printf("SetMessage() releasing mutex\n");
        if (!ReleaseMutex(m_hMutex))   // Jaeschke's Monitor::Exit(this)
        {
            printf("ReleaseMutex() failed in MessageBuffer::SetMessage().\n");
        }
#endif
    }

    void ProcessMessages()
    {
        //        printf("in MessageBuffer::ProcessMessages()\n");

        while (m_bContinueProcessing)   // state variable used to control thread lifetime
        {
            // We now want to enter an "alertable wait state" so that
            // this consumer thread doesn't burn any cycles except
            // upon those occasions when the producer thread indicates
            // that a message waits for processing.

#ifdef WITH_SYNCHRONIZATION
            DWORD   dwWaitResult = WaitForSingleObject(m_hEvent, 2000);

            if ((dwWaitResult == WAIT_TIMEOUT)   // WAIT_TIMEOUT = 258
                && (m_bContinueProcessing == false))
            {
                break;    // we were told to die
            }
            else if (dwWaitResult == WAIT_ABANDONED) // WAIT_ABANDONED = 80
            {
                printf("WaitForSingleObject(1) failed in MessageBuffer::ProcessMessages().\n");
                return;
            }
            else if (dwWaitResult == WAIT_OBJECT_0)  // WAIT_OBJECT_0 = 0
            {
                printf("ProcessMessages() saw Event\n");
            }

            dwWaitResult = WaitForSingleObject(m_hMutex, INFINITE);

            if (dwWaitResult != WAIT_OBJECT_0)
            {
                printf("WaitForSingleObject(2) failed in MessageBuffer::ProcessMessages().\n");
                return;
            }
            printf("ProcessMessages() acquired mutex\n");
#endif

            if (strlen(messageText) != 0)
            {
                printf("Processed new message: %s\n", messageText);

                // We now empty the message buffer to show we have finished
                // processing the message:

                messageText[0] = '\0';
            }

#ifdef WITH_SYNCHRONIZATION
            printf("ProcessMessages() releasing mutex\n");
            if (!ReleaseMutex(m_hMutex))
            {
                printf("ReleaseMutex() failed in MessageBuffer::ProcessMessages().\n");
            }
#endif

        } // end of while ( m_bContinueProcessing ) loop
    }

    void  DieDieDie(void)
    {
        m_bContinueProcessing = false;   // ProcessMessages() watches for this in a loop
    }
};

class CreateMessages        // this is the producer
{
    MessageBuffer* msg;
public:
    CreateMessages(MessageBuffer* m)    // ctor
    {
        msg = m;
    }

    void CreateMessagesEntryPoint()
    {
        char  szTemp[MSG_BUF_SIZE];

        // We create 5 string messages for the consumer to process.
        // These 5 messages look like:
        //    1111111111
        //    2222222222
        //    3333333333
        //    4444444444
        //    5555555555

        for (int i = 1; i <= 5; ++i)
        {
            sprintf_s(szTemp, "%d%d%d%d%d%d%d%d%d%d", i, i, i, i, i, i, i, i, i, i);
            //          printf( "new msg = %s\n", szTemp );

            msg->SetMessage(szTemp);
            Sleep(1000);
        }

        // Once we have created all the messages we die by
        // exiting our thread entry-point-function.

        printf("CreateMessages thread terminating.\n");
    }

    // In C++ you must employ a free (C) function or a
    // static class member function as the thread entry function.
    // Furthermore, _beginthreadex() demands that the thread
    // entry function signature take a single (void*) and returned
    // an unsigned.

    static unsigned __stdcall CreateMessagesStaticEntryPoint(void * pThis)
    {
        CreateMessages* pCM = (CreateMessages*)pThis;
        pCM->CreateMessagesEntryPoint();

        // Unlike the consumer thread which attempts to live forever,
        // this producer thread commits suicide after performing a
        // specific amount of work.

        // You can call _endthreadex() explicitly to terminate a thread;
        // however, _endthreadex() is called automatically when the thread
        // returns from the routine passed as a parameter to _beginthreadex().

        // By calling _endthreadex() we cause our thread object to go
        // to the signaled state, which releases anyone waiting on us.

        _endthreadex(2);    // the thread exit code

                            // _endthreadex() does not close the thread handle so somebody
                            // must still call CloseHandle().

                            // We never get to here because of the earlier call to _endthreadex().

        return 2;  // the thread exit code
    }
};

class ProcessMessages   // this is the consumer
{
    MessageBuffer* msg;
public:
    ProcessMessages(MessageBuffer* m)    // ctor
    {
        msg = m;
    }

    void ProcessMessagesEntryPoint()
    {
        msg->ProcessMessages();
    }

    static unsigned __stdcall ProcessMessagesStaticEntryPoint(void * pThis)
    {
        ProcessMessages * pPM = (ProcessMessages*)pThis;
        pPM->ProcessMessagesEntryPoint();  // calls MessageBuffer::ProcessMessages()
                                           // which has a while(m_bContinueProcessing) loop

        printf("ProcessMessages thread terminating.\n");
        return 1;   // the thread exit code
    }

    void  DieDieDie(void)
    {
        msg->DieDieDie();   // pass the die message along to the MessageBuffer class
                            // which watches for it in its ProcessMessages() function
    }
};

int main()
{
    DWORD   dwExitCode;

    // The following single instance of the MessageBuffer class is shared
    // by both the producer and consumer threads.  Via synchronization
    // statements, this program guarantees that the consumer thread can't
    // process the contents of the message buffer until the producer thread
    // has put something there, and that the producer thread can't
    // put another message there until the previous one has been consumed.

    MessageBuffer* m = new MessageBuffer();

    ProcessMessages* pm = new ProcessMessages(m);

    HANDLE   hthConsumer;
    unsigned  uiConsumerThreadID;

    // When developing a multithreaded WIN32-based application with
    // Visual C++, you need to use the CRT thread functions to create
    // any threads that call CRT functions. Hence to create and terminate
    // threads, use _beginthreadex() and _endthreadex() instead of
    // the Win32 APIs CreateThread() and EndThread().

    // Unlike the thread handle returned by _beginthread(), the thread handle
    // returned by _beginthreadex() can be used with the synchronization APIs.

    hthConsumer = (HANDLE)_beginthreadex(NULL,
        0,
        ProcessMessages::ProcessMessagesStaticEntryPoint,
        pm,
        CREATE_SUSPENDED, // so we can later call ResumeThread()
        &uiConsumerThreadID);

    if (hthConsumer == 0)
        printf("Failed to create consumer thread\n");

    GetExitCodeThread(hthConsumer, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial Consumer thread exit code = %u\n", dwExitCode);


    ResumeThread(hthConsumer);   // Jaeschke's pmt->Start()


    CreateMessages* cm = new CreateMessages(m);  // note that both the ProcessMessages ctor
                                                 // and the CreateMessages ctor are given
                                                 // the same passed param. Hence they share
                                                 // a single MessageBuffer instance.

    HANDLE  hthProducer;
    unsigned  uiProducerThreadID;
    hthProducer = (HANDLE)_beginthreadex(NULL,
        0,
        CreateMessages::CreateMessagesStaticEntryPoint,
        cm,
        CREATE_SUSPENDED,
        &uiProducerThreadID);
    if (hthProducer == 0)
        printf("Failed to create producer thread\n");

    GetExitCodeThread(hthProducer, &dwExitCode);  // should be STILL_ACTIVE = 0x00000103 = 259
    printf("initial Producer thread exit code = %u\n", dwExitCode);

    ResumeThread(hthProducer);   // Jaeschke's cmt->Start()

                                 // Now that all threads are running, we wait for the producer thread
                                 // to finish its work and die.

                                 // Unlike the consumer thread which attempts to live forever,
                                 // this producer thread commits suicide after performing a
                                 // specific amount of work.

    WaitForSingleObject(hthProducer, INFINITE);  // Jaeschke's cmt->Join()

    GetExitCodeThread(hthProducer, &dwExitCode);
    printf("Producer thread exited with code %u\n", dwExitCode);

    // We now explicitly command the consumer thread to die.

    pm->DieDieDie();       // Jaeschke's pmt->Interrupt()

                           // We now wait for the consumer thread to die.

    WaitForSingleObject(hthConsumer, INFINITE);  // Jaeschke's pmt->Join()

    GetExitCodeThread(hthConsumer, &dwExitCode);
    printf("Consumer thread exited with code %u\n", dwExitCode);

    // The handle returned by _beginthreadex() has to be closed
    // by the caller of _beginthreadex().

    CloseHandle(hthProducer);
    CloseHandle(hthConsumer);

    delete cm;
    cm = NULL;

    delete pm;
    pm = NULL;

    delete  m;
    m = NULL;

    printf("Primary thread terminating.\n");
}

