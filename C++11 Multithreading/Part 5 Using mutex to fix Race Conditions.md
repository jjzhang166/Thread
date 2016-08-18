# Part 5: Using mutex to fix Race Conditions

In this article we will discuss how to use mutex locks to protect shared data in multithreaded environment and avoid race conditions.

To fix race conditions in multi-threaded environment we need mutex i.e. each thread needs to lock a mutex before modifying or reading the shared data and after modifying the data each thread should unlock the mutex.

In the C++11 threading library, the mutexes are in the <mutex> header file. The class representing a mutex is the std::mutex class.

There are two important methods of mutex:
1.) lock()
2.) unlock()

We have explained Race condition using a Multithreaded Wallet in previous article i.e.

**http://thispointer.com/c11-multithreading-part-4-data-sharing-and-race-conditions/**

In this article we will see how to use mutex to fix the race condition in that multithreaded wallet.

As, Wallet provides a service to add money in Wallet and same Waqllet object is used between different threads, so we need to add Lock in addMoney() method of the Wallet i.e.
Acquire lock before incrementing the money of Wallet and release lock before leaving that function. Let’s see the code,

Wallet class that internally maintains money and provides a service/function i.e. addMoney().
This member function, first acquires a lock then increments the internal money of wallet object by specified count and then releases the lock.

```c++
#include<iostream>
#include<thread>
#include<vector>
#include<mutex>

class Wallet
{
	int mMoney;
	std::mutex mutex;
public:
	Wallet() :mMoney(0){}
    int getMoney()   { 	return mMoney; }
    void addMoney(int money)
    {
		mutex.lock();
    	for(int i = 0; i < money; ++i)
		{
			mMoney++;
		}
		mutex.unlock();
    }
};
```

Now Let’s create 5 threads and all these threads will share a same object of class Wallet and add 1000 to internal money using it’s addMoney() member function in parallel.

So, if initially money in wallet is 0. Then after completion of all thread’s execution money in Wallet should be 5000.

And this mutex lock guarantees that Money in the Wallet will be 5000 at end.

Let’s test this,

```c++
int testMultithreadedWallet()
{
    Wallet walletObject;
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i){
        threads.push_back(std::thread(&Wallet::addMoney, &walletObject, 1000));
    }

    for(int i = 0; i < threads.size() ; i++)
    {
        threads.at(i).join();
    }
    return walletObject.getMoney();
}

int main()
{

	int val = 0;
	for(int k = 0; k < 1000; k++)
	{
		if((val = testMultithreadedWallet()) != 5000)
		{
			std::cout << "Error at count = "<<k<<"  Money in Wallet = "<<val << std::endl;
			//break;
		}
	}
	return 0;
}
```

It’s guaranteed that it will not found a single scenario where money in wallet is less than 5000.
Because mutex lock in addMoney makes sure that once one thread finishes the modification of money then only any other thread modifies the money in Wallet.

But what if we forgot to unlock the mutex at the end of function. In such scenario, one thread will exit without releasing the lock and other threads will remain in waiting.
This kind of scenario can happen in case some **exception** came after locking the mutex. To avoid such scenarios we should use std::lock_guard.

**std::lock_guard :**

std::lock_guard is a class template, which implements the RAII for mutex.
It wraps the mutex inside it’s object and locks the attached mutex in its constructor. When it’s destructor is called it releases the mutex.

Let’s see the code,

```c++
class Wallet
{
	int mMoney;
	std::mutex mutex;
public:
    Wallet() :mMoney(0){}
    int getMoney()   { 	return mMoney; }
    void addMoney(int money)
    {
		std::lock_guard<std::mutex> lockGuard(mutex);
		// In constructor it locks the mutex

    	for(int i = 0; i < money; ++i)
		{
			// If some exception occurs at this
			// poin then destructor of lockGuard
			// will be called due to stack unwinding.
			//
			mMoney++;
		}
		// Once function exits, then destructor
		// of lockGuard Object will be called.
		// In destructor it unlocks the mutex.
    }
 };
```