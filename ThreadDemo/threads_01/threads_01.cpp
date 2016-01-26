// threads_01.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    //Use of an anonymous function (lambda) in a thread
    thread t([](string name) {
        cout << "Hello " << name << endl;
    }, "Tom");

    //Join the thread with the main thread
    t.join();

    return 0;
}

