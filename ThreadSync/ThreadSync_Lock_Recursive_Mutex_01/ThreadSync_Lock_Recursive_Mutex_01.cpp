// ThreadSync_Lock_Recursive_Mutex_01.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

#include <stdarg.h>

template <typename T>
class container
{
    std::recursive_mutex _lock; // When you execute this program it runs into a deadlock.
                                // The reason is the container attempts to acquire the mutex multiple times before releasing it and that is not possible.
                                // That is where std::recursive_mutex come into the picture.
                                // It allows a thread to acquire the same mutex multiple times.
                                // The maximum number of times the mutex can be acquired is not specified,
                                // but if that number is reached, calling lock would throw a std::system_error.
                                // Therefore to fix the problem in the code above (apart from changing the implementation of addrange not to calllock and unlock)
                                // is to replace the mutex with a std::recursive_mutex.

    std::vector<T> _elements;
public:
    void add(T element)
    {
        std::lock_guard<std::recursive_mutex> locker(_lock);
        _elements.push_back(element);
    }

    void addrange(int num, ...)
    {
        va_list arguments;

        va_start(arguments, num);

        for (int i = 0; i < num; i++)
        {
            std::lock_guard<std::recursive_mutex> locker(_lock);
            add(va_arg(arguments, T));
        }

        va_end(arguments);
    }

    void dump()
    {
        std::lock_guard<std::recursive_mutex> locker(_lock);
        for (auto e : _elements)
            std::cout << e << std::endl;
    }
};

void func(container<int>& cont)
{
    cont.addrange(3, rand(), rand(), rand());
}

int main()
{
    srand((unsigned int)time(0));

    container<int> cont;

    std::thread t1(func, std::ref(cont));
    std::thread t2(func, std::ref(cont));
    std::thread t3(func, std::ref(cont));

    t1.join();
    t2.join();
    t3.join();

    cont.dump();

    return 0;
}
