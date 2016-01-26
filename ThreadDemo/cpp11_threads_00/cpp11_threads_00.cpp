// cpp11_threads_00.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>
#include <vector>

//This function will be called from a thread

void func(int tid)
{
    std::cout << "Launched by thread " << tid << std::endl;
}

int main()
{
    std::vector<std::thread> th;

    int nr_threads = 10;

    //Launch a group of threads
    for (int i = 0; i < nr_threads; ++i)
    {
        th.push_back(std::thread(func, i));
    }

    //Join the threads with the main thread
    for (auto &t : th)
    {
        t.join();
    }

    return 0;
}
