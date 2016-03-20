#pragma once

#include <queue>
#include <thread>
#include <vector>
#include <functional>

namespace std
{
    class mutex;
    class condition_variable;
}//namespace std

namespace Utilities
{
    class Semaphore final
    {
    private:
        std::condition_variable* _condition;
        std::mutex*              _mutex;
        unsigned                 _count;

    public:
        Semaphore();
        ~Semaphore();

    public:
        void Wait();
        void Notify();
    };

    class Worker final
    {
    public:
        typedef std::function<void()> Task;

    private:
        std::queue<Task> _tasks;
        std::thread*     _thread;
        std::mutex*      _mutex;
        Semaphore        _semaphore;
        bool             _alive;

    public:
        Worker();
        ~Worker();

    public:
        std::thread::id id() const;
        size_t          task_count() const;

        void Join() const;
        void PushTask(Task task);
        void ClearTasks();
    };

    class ThreadPool final
    {
    private:
        std::vector<Worker> _workers;
        unsigned            _index;

    public:
        ThreadPool(unsigned thread_count = 0);
        ~ThreadPool();

    public:
        unsigned thread_count();
        unsigned task_count();

        void Join();
        void PushTask(Worker::Task task);
        void ClearTasks();
    };
}//namespace Utilities