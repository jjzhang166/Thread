// ThreadSync_Lock_Lock_Mutex.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <set>
#include <chrono>

template <typename T>
class container
{
public:
    std::mutex _lock;
    std::set<T> _elements;

    void add(T element)
    {
        _elements.insert(element);
    }

    void remove(T element)
    {
        _elements.erase(element);
    }
};

// Suppose this function is called from two different threads,
// from the first, an element is removed from container 1 and added to container 2,
// and in the second it is removed from container 2 and added to container 1.
// This can lead to a deadblock (if the thread context switches from one thread to another just after acquiring the first lock).

void exchange(container<int>& cont1, container<int>& cont2, int value)
{
    //cont1._lock.lock();
    //std::this_thread::sleep_for(std::chrono::seconds(1)); // <-- forces context switch to simulate the deadlock
    //cont2._lock.lock();

    std::lock(cont1._lock, cont2._lock); // use std::lock that guaranties the locks are acquired in a deadlock-free way

    cont1.remove(value);
    cont2.add(value);

    cont1._lock.unlock();
    cont2._lock.unlock();
}


int main()
{
    srand((unsigned int)time(NULL));

    container<int> cont1;
    cont1.add(1);
    cont1.add(2);
    cont1.add(3);

    container<int> cont2;
    cont2.add(4);
    cont2.add(5);
    cont2.add(6);

    std::thread t1(exchange, std::ref(cont1), std::ref(cont2), 3);
    std::thread t2(exchange, std::ref(cont2), std::ref(cont1), 6);

    t1.join();
    t2.join();

    std::set<int>::iterator iter1 = cont1._elements.begin();
    for (; iter1 != cont1._elements.end(); ++iter1)
    {
        std::cout << *iter1 << " ";
    }
    std::cout << std::endl;


    std::set<int>::iterator iter2 = cont2._elements.begin();
    for (; iter2 != cont2._elements.end(); ++iter2)
    {
        std::cout << *iter2 << " ";
    }
    std::cout << std::endl;

    return 0;
}