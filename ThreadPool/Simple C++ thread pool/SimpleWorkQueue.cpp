#include "stdafx.h"
#include "SimpleWorkQueue.h"

SimpleWorkQueue::SimpleWorkQueue(int initialThreadCount)
    : stopping(false)
    , finished(false)
{
    std::condition_variable     constructBarrier;
    int count = 0;

    workers.reserve(initialThreadCount);
    for (int loop = 0; loop < initialThreadCount; ++loop)
    {
        /*
        * Create all the threads we want in this loop.
        * For each thread we also add a job so that we
        * make sure it starts up correctly.
        *
        * To make sure that each thread only adds one to
        * the count the thread is held in the action
        * until all the threads have started up
        */
        work.push_back([&constructBarrier, &count, this]()
        {
            std::unique_lock<std::mutex> locker(this->lock);
            ++count;                            // Add one to the count
            constructBarrier.notify_one();      // And notify the Work-Queue.

            this->threadBarrier.wait(locker);   // Wait until all threads
            // have started.
        });
        // Create a thread to deal with the job we just started.
        workers.emplace_back(&SimpleWorkQueue::workerAction, this);
    }
    /*
    * We have created all the threads.
    * But we must now wait for all threads to
    * enter the while loop inside `workerAction()`
    *
    * We want to make sure that all threads have entered
    * the while loop before the destructor is entered
    * because if the destructor is entered we could
    * set finished to false before the thread enters the
    * function which makes reasoning about exit conditions
    * very hard.
    */
    std::unique_lock<std::mutex> locker(lock);
    constructBarrier.wait(locker, [&count, this](){
        return count == this->workers.size();
    });

    /*
    * All the thread have entered the check above and
    * incremented the counter. This released this thread
    * We can now release all the threads to start accepting
    * work items.
    */
    threadBarrier.notify_all();
}

SimpleWorkQueue::~SimpleWorkQueue()
{
    tellAllThreadsToStop();

    cond.notify_all();
    for (std::thread& thread : workers)
    {
        thread.join();
    }
}

void SimpleWorkQueue::tellAllThreadsToStop()
{
    std::unique_lock<std::mutex> locker(lock);
    stopping = true;
    /*
    * Push one job for each thread.
    * After this job is executed it will exit the while loop
    * in `workerAction()` (because finished is true)
    * thus causing the thread to exit.
    *
    * Thus each thread will execute one of these jobs then exit.
    * Thus all threads will eventually die.
    *
    * We do it this way to make sure that all work that has
    * been added will be completed before the threads start
    * exiting.
    */
    for (std::thread& thread : workers)
    {
        work.push_back([this](){
            std::unique_lock<std::mutex> locker(this->lock);
            this->finished = true;
        });
    }
}

void SimpleWorkQueue::addWorkItem(std::function<void()> action)
{
    std::unique_lock<std::mutex> locker(lock);
    if (stopping) {
        throw std::runtime_error("Can't add work after destructor entered");
    }
    work.push_back(action);
    cond.notify_one();
}

void SimpleWorkQueue::workerAction()
{
    while (!finished)
    {
        std::function<void()> item = getNextWorkItem();
        try
        {
            item();
        }
        catch (...)
        {
            /*
            * Must catch all exceptions generate by work.
            * If you don't then the user code may cause the
            * thread to accidentally exit.
            *
            * In old threading systems (like pthreads)
            * it was undefined behavior if the thread exited
            * with an exception. That's probably not the case here
            *
            * But you definitely don't want the thread to end
            * because of an exception in user space. So
            * catch and throw away (log if you must).
            */
        }
    }
}

std::function<void()> SimpleWorkQueue::getNextWorkItem()
{
    std::unique_lock<std::mutex> locker(lock);
    cond.wait(locker, [this](){return !this->work.empty(); });

    std::function<void()> result = work.front();
    work.pop_front();
    return result;
}