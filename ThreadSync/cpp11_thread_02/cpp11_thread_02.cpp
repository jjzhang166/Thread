// cpp11_thread_02.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <thread>
#include <iostream>

class ThreadRAII
{
    std::thread & m_thread;
public:
    ThreadRAII(std::thread  & threadObj) : m_thread(threadObj)
    {

    }
    ~ThreadRAII()
    {
        // Check if thread is joinable then detach the thread
        if (m_thread.joinable())
        {
            m_thread.detach();
        }
    }
};
void thread_function()
{
    for (int i = 0; i < 10000; i++);
    {
        std::cout << "thread_function Executing" << std::endl;
    }
}

int main()
{
    std::thread threadObj(thread_function);

    // If we comment this Line, then program will crash
    ThreadRAII wrapperObj(threadObj);

    return 0;
}
