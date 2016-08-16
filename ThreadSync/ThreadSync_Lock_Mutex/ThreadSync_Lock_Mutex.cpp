// ThreadSync.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex g_lock;

void func()
{
    g_lock.lock();

    std::cout << "entered thread " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(rand() % 10));
    std::cout << "leaving thread " << std::this_thread::get_id() << std::endl;

    g_lock.unlock();
}

int global_counter = 0;
std::mutex counter_mutex;

void five_thread_fn() {
    for (int i = 0; i < 5; i++) {
        counter_mutex.lock();
        global_counter++;
        counter_mutex.unlock();
        std::cout << "Updated from five_thread" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    //When this thread finishes we wait for it to join
}

void ten_thread_fn() {
    for (int i = 0; i < 10; i++) {
        counter_mutex.lock();
        global_counter++;
        counter_mutex.unlock();
        std::cout << "Updated from ten_thread" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    //When this thread finishes we wait for it to join
}

int main()
{
    srand((unsigned int)time(0));

    std::thread t1(func);
    std::thread t2(func);
    std::thread t3(func);

    t1.join();// wait for t1 to finish(block until func() ends)
    t2.join();
    t3.join();


    std::cout << "starting thread ten..." << std::endl;
    std::thread ten_thread(ten_thread_fn);

    std::cout << "Running ten thread" << std::endl;
    std::thread five_thread(five_thread_fn);


    ten_thread.join();
    std::cout << "Ten Thread is done." << std::endl;
    five_thread.join();
    std::cout << "Five Thread is done." << std::endl;

    int flag = getchar();

    return 0;
}

