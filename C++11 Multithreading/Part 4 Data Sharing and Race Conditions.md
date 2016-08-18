# Part 4: Data Sharing and Race Conditions

In multithreading environment data sharing between threads is very easy. But this easy sharing of data can cause problems in application. One such problem is *Race Condition*.

**What is a Race Condition?**

Race condition is a kind of a bug that occurs in multithreaded applications.

When two or more threads perform a set of operations in parallel, that access the same memory location.  Also, one or more thread out of them modifies the data in that memory location, then this can lead to an unexpected results some times.

This is called race condition.

Race conditions are usually hard to find and reproduce because they don’t occur every time. They will occur only when relative order of execution of operations by two or more threads leads to an unexpected result. Let’s understand by an example,

**A Practical example of Race Condition:**

Let’s Create a Wallet class that internally maintains money and provides a service/function i.e. addMoney(). This member function increments the internal money of wallet object by specified count.

```c++
class Wallet
{
    int mMoney;
public:
    Wallet() :mMoney(0){}
    int getMoney() { return mMoney; }
    void addMoney(int money)
    {
       for(int i = 0; i < money; ++i)
       {
          mMoney++;
       }
    }
};
```

Now Let’s create 5 threads and all these threads will share a same object of class Wallet and add 1000 to internal money using it’s addMoney() member function in parallel.

So, if initially money in wallet is 0. Then after completion of all thread’s execution money in Wallet should be 5000.

But, as all threads are modifying the shared data at same time, it might be possible that in some scenarios money in wallet at end will be much lesser than 5000.

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
       std::cout << "Error at count = "<<k<<" Money in Wallet = "<<val << std::endl;
     }
  }
  return 0;
}
```

As addMoney() member function of same Wallet class object is executed 5 times hence it’s internal money is expected to be 5000. But as addMoney() member function is executed in parallel hence in some scenarios mMoney will be much lesser than 5000 i.e.

**Output is,**

**Error at count = 971  Money in Wallet = 4568                                                                                                             Error at count = 971  Money in Wallet = 4568                                                                                                             Error at count = 972  Money in Wallet = 4260                                                                                                             Error at count = 972  Money in Wallet = 4260                                                                                                             Error at count = 973  Money in Wallet = 4976                                                                                                             Error at count = 973  Money in Wallet = 4976**

This is a race condition, as here two or more threads were trying to modify the same memory location at same time and lead to unexpected result.

**Why this happened?**

Each thread increments the same “mMoney” member variable in parallel. Although it seems a single line but this “mMoney++” is actually converted into three machine commands,

- Load “mMoney” variable value in Register
- Increment register’s value
- Update variable “mMoney” with register’s value

Now suppose in a special scenario, order of execution of above these commands is as follows,

 

[![Order of Executions Of Commands](http://thispointer.com/wp-content/uploads/2015/02/thread_order1.jpg)](http://thispointer.com/wp-content/uploads/2015/02/thread_order1.jpg)Order of Executions Of Commands

In this scenario one increment will get neglected because instead of incrementing the “mMoney” variable twice, different registers got incremented and “mMoney” variable’s value was overwritten.

Suppose prior to this scenario mMoney was 46 and as seen in above image it is incremented 2 times, so expected result is 48. But due to race condition in above scenario final value of mMoney will be 47 only.

This is called a Race Condition.

**How to fix Race Conditions?**

To fix this problem we need to use Lock mechanism i.e. each thread need to acquire a lock before modifying or reading the shared data and after modifying the data each thread should unlock the Lock.

We will discuss this in next article i.e.

http://thispointer.com/c11-multithreading-part-5-using-mutex-to-fix-race-conditions/