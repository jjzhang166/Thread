#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>

class SimpleWorkQueue
{
public:
    SimpleWorkQueue(int initialThreadCount);
    ~SimpleWorkQueue();
    void addWorkItem(std::function<void()> action);

private:
    bool                                stopping;
    bool                                finished;
    std::mutex                          lock;
    std::condition_variable             cond;
    std::condition_variable             threadBarrier;
    std::list<std::function<void()>>    work;
    std::vector<std::thread>            workers;

    std::function<void()> getNextWorkItem();
    void workerAction();
    void tellAllThreadsToStop();
};