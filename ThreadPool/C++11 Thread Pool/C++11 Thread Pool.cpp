// C++11 Thread Pool.cpp : 定义控制台应用程序的入口点。
//

//http://codereview.stackexchange.com/questions/88042/c11-thread-pool?rq=1

#include "stdafx.h"

#include "ThreadPool.h"

#include <iostream>

static void Method_0()
{
    std::cout << "Method_0()" << std::endl;
}

static void Method_1(float my_float)
{
    std::cout << my_float << std::endl;
}

static void Method_2(int&  my_int)
{
    my_int += my_int;
}

struct Foo
{
    void Bar()
    {
        std::cout << "Foo::Bar() : Instance" << std::endl;
    }

    static void Baz()
    {
        std::cout << "Foo::Baz() : Static" << std::endl;
    }
};

int main(int argc, char* argv[])
{
    using namespace Utilities;

    Foo foo;
    int my_int = 16;

    ThreadPool thread_pool;
    thread_pool.PushTask(Method_0);
    thread_pool.PushTask(std::bind(Method_1, 3.14f));
    thread_pool.PushTask(std::bind(Method_2, std::ref(my_int)));
    thread_pool.PushTask(std::bind(&Foo::Bar, foo));
    thread_pool.PushTask(&Foo::Baz);
    thread_pool.PushTask([] { std::cout << "Lambda" << std::endl; });
    thread_pool.Join();

    std::cout << my_int << std::endl;

    return 0;
}
