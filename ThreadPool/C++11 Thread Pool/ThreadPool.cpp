#include "stdafx.h"

#include "ThreadPool.h"

#include <mutex>
#include <algorithm>
#include <condition_variable>

using namespace Utilities;

Semaphore::Semaphore() :
    _condition(new std::condition_variable()),
    _mutex(new std::mutex()),
    _count(0)
{ }

Semaphore::~Semaphore()
{
    delete _condition;
    delete _mutex;
}

void Semaphore::Wait()
{
    std::unique_lock<std::mutex> lock(*_mutex);
    while (!_count)
        _condition->wait(lock);
    --_count;
}//Wait

void Semaphore::Notify()
{
    std::unique_lock<std::mutex> lock(*_mutex);
    _condition->notify_one();
    ++_count;
}//Notify

Worker::Worker() :
    _mutex(new std::mutex()),
    _alive(true)
{
    _thread = new std::thread(
        [&]
    {
        while (_alive)
        {
            _semaphore.Wait();

            while (!_tasks.empty())
            {
                _mutex->lock();
                Task task = _tasks.front();
                _mutex->unlock();

                task();

                _mutex->lock();
                if (!_tasks.empty())
                    _tasks.pop();
                _mutex->unlock();
            }
        }
    });
}

Worker::~Worker()
{
    ClearTasks();
    _alive = false;
    _semaphore.Notify();

    _thread->join();

    delete _thread;
    delete _mutex;
}

std::thread::id Worker::id() const
{
    return _thread->get_id();
}
//id

size_t Worker::task_count() const
{
    return _tasks.size();
}
//task_count

void Worker::Join() const
{
    while (_tasks.size());
}
//Join

void Worker::PushTask(Task task)
{
    _mutex->lock();
    _tasks.push(task);
    _mutex->unlock();

    _semaphore.Notify();
}//PushTask

void Worker::ClearTasks()
{
    _mutex->lock();
    _tasks = std::queue<Task>();
    _mutex->unlock();
}//ClearTasks

ThreadPool::ThreadPool(unsigned thread_count) :
    _index(0)
{
    if (!thread_count)
        thread_count = std::thread::hardware_concurrency();
    _workers = std::vector<Worker>(std::max(1u, thread_count));
}

ThreadPool::~ThreadPool()
{
    ClearTasks();
    Join();
}

unsigned ThreadPool::thread_count()
{
    return _workers.size();
}
//thread_count

unsigned ThreadPool::task_count()
{
    unsigned task_count = 0;
    for (auto& worker : _workers)
        task_count += worker.task_count();
    return task_count;
}//task_count

void ThreadPool::Join()
{
    while (task_count());
}
//Join

void ThreadPool::PushTask(Worker::Task task)
{
    _workers[_index++].PushTask(task);
    if (_index >= _workers.size())
        _index = 0;
}//PushTask

void ThreadPool::ClearTasks()
{
    for (auto& worker : _workers)
        worker.ClearTasks();
}//ClearTasks