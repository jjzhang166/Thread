// cpp11_thread_01.cpp : 定义控制台应用程序的入口点。
//

// if a thread function throws an exception it won't be caught with a regular try-catch block. In other words, this won't work:
/*
    try
    {
        std::thread t1(func);
        std::thread t2(func);

        t1.join();
        t2.join();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
*/

#include "stdafx.h"

#include <mutex>
#include <vector>
#include <iostream>

std::mutex                       g_mutex;
std::vector<std::exception_ptr>  g_exceptions;

void throw_function()
{
    throw std::exception("something wrong happened");
}

void func()
{
    try
    {
        throw_function();
    }
    catch (...)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_exceptions.push_back(std::current_exception());
    }
}

int main()
{
    g_exceptions.clear();

    std::thread t(func);
    t.join();

    for (auto& e : g_exceptions)
    {
        try
        {
            if (e != nullptr)
            {
                std::rethrow_exception(e);
            }
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}