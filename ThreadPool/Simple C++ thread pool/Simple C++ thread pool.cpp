// Simple C++ thread pool.cpp : �������̨Ӧ�ó������ڵ㡣
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

