// cpp_thread_01.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>

static const int num_threads = 10;

//This function will be called from a thread
void call_from_thread()
{
    std::cout << "Launched by thread\n";
}

int main()
{
    std::thread t[num_threads];

    //Launch a group of threads
    for (int i = 0; i < num_threads; ++i)
    {
        t[i] = std::thread(call_from_thread);
    }

    std::cout << "Launched from the main\n";

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i)
    {
        t[i].join();
    }

    return 0;
}
