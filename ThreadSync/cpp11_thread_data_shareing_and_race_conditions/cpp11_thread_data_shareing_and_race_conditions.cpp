// cpp11_thread_data_shareing_and_race_conditions.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <thread>
#include <vector>

class Wallet
{
    int mMoney;
public:
    Wallet()
        : mMoney(0)
    {

    }

    int getMoney()
    {
        return mMoney;
    }

    void addMoney(int money)
    {
        for (int i = 0; i < money; ++i)
        {
            mMoney++;
        }
    }
};

int testMultithreadedWallet()
{
    Wallet walletObject;
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.push_back(std::thread(&Wallet::addMoney, &walletObject, 1000));
    }

    for (size_t i = 0; i < threads.size(); i++)
    {
        threads.at(i).join();
    }
    return walletObject.getMoney();
}

int main()
{
    int val = 0;
    for (int k = 0; k < 1000; k++)
    {
        if ((val = testMultithreadedWallet()) != 5000)
        {
            std::cout << "Error at count = " << k << " Money in Wallet = " << val << std::endl;
        }
    }
    return 0;
}

