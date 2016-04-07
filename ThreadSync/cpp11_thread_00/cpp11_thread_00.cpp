// cpp11_thread_00.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <thread>
#include <string>
#include <iostream>

void func()
{
    std::cout << "for test" << std::endl;
}

int main()
{
    std::thread t1(func);
    std::cout << "main" << std::endl;

    t1.detach();// allows a thread of execution to continue independently of the thread object.
                // Detached threads are no longer joinable (you cannot wait for them).

    return 0;
}

