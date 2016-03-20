// Simple C++ thread pool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SimpleWorkQueue.h"

using namespace std;

#include <iostream>

int main()
{
    SimpleWorkQueue queue(10);

    queue.addWorkItem([](){std::cerr << "Hi" << time(NULL) << "\n"; });

    getchar();

    return 0;
}

