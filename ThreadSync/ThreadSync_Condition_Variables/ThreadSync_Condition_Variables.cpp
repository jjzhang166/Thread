// ThreadSync_Condition_Variables.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <random>


/*
The following describes how condition variables work:

1、There must be at least one thread that is waiting for a condition to become true.The waiting thread must first acquire a unique_lock.
   This lock is passed to the wait() method, that releases the mutex and suspends the thread until the condition variable is signaled.
   When that happens the thread is awaken and the lock is re-acquired.
2、There must be at least one thread that is signaling that a condition becomes true.
   The signaling can be done with notify_one() which unblocks one thread (any) that is waiting for the condition to be signaled or with notify_all()
   which unblocks all the threads that are waiting for the condition.
3、Because of some complications in making the condition wake-up completely predictable on multiprocessor systems,spurious wake-ups can occur.
   That means a thread is awaken even if nobody signaled the condition variable.
   Therefore it is necessary to check if the condition is still true after the thread has awaken.
   And since spurious wake-ups can occur multiple times, that check must be done in a loop.




*/

std::mutex              g_lockprint;
std::mutex              g_lockqueue;
std::condition_variable g_queuecheck;
std::queue<int>         g_codes;
bool                    g_done;
bool                    g_notified;

void workerfunc(int id, std::mt19937& generator)
{
    // print a starting message
    {
        std::unique_lock<std::mutex> locker(g_lockprint);
        std::cout << "[worker " << id << "]\trunning..." << std::endl;
    }

    // simulate work
    std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));

    // simulate error
    int errorcode = id * 100 + 1;
    {
        std::unique_lock<std::mutex> locker(g_lockprint);
        std::cout << "[worker " << id << "]\tan error occurred: " << errorcode << std::endl;
    }

    // notify error to be logged
    {
        std::unique_lock<std::mutex> locker(g_lockqueue);
        g_codes.push(errorcode);
        g_notified = true;
        g_queuecheck.notify_one();
    }
}

void loggerfunc()
{
    // print a starting message
    {
        std::unique_lock<std::mutex> locker(g_lockprint);
        std::cout << "[logger]\trunning..." << std::endl;
    }

    // loop until end is signaled
    while (!g_done)
    {
        std::unique_lock<std::mutex> locker(g_lockqueue);

        while (!g_notified) // used to avoid spurious wakeups 
        {
            g_queuecheck.wait(locker);
        }

        // if there are error codes in the queue process them
        while (!g_codes.empty())
        {
            std::unique_lock<std::mutex> locker(g_lockprint);
            std::cout << "[logger]\tprocessing error:  " << g_codes.front() << std::endl;
            g_codes.pop();
        }

        g_notified = false;
    }
}

int main()
{
    // initialize a random generator
    std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

    // start the logger
    std::thread loggerthread(loggerfunc);

    // start the working threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(workerfunc, i + 1, std::ref(generator)));
    }

    // work for the workers to finish
    for (auto& t : threads)
        t.join();

    // notify the logger to finish and wait for it
    g_done = true;
    loggerthread.join();

    return 0;
}

