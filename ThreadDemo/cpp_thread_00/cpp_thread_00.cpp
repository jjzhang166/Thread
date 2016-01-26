// cpp_thread_00.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>

//This function will be called from a thread
void call_from_thread()
{
    std::cout << "Hello, World!" << std::endl;
}

int main()
{
    //Launch a thread
    std::thread t1(call_from_thread);

    std::cout << "main " << std::endl;

    //Join the thread with the main thread
    t1.join();

    return 0;
}