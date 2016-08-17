# Part 3: Carefully Pass Arguments to Threads

To Pass arguments to thread’s associated callable object or function just pass additional arguments to the std::thread constructor.
By default all arguments are copied into the internal storage of new thread.

默认情况下，所有的参数都会复制到线程内部

Lets look at an example

**Passing simple arguments to a thread **

```c++
#include <iostream>
#include <string>
#include <thread>

void threadCallback(int x, std::string str)
{
    std::cout<<"Passed Number = "<<x<<std::endl;
    std::cout<<"Passed String = "<<str<<std::endl;
}

int main()  
{
    int x = 10;
    std::string str = "Sample String";
    std::thread threadObj(threadCallback, x, str);
    threadObj.join();
    return 0;
}
```

**How not to pass arguments to threads:**

不要传递栈上变量的地址到线程的回调函数

Don’t pass addresses of variables from local stack to thread’s callback function. Because it might be possible that local variable in Thread 1 goes out of scope but Thread 2 is still trying to access it through it’s address.
In such scenario accessing invalid address can cause unexpected behaviour.

For example,

```c++
#include <iostream>
#include <thread>

void newThreadCallback(int * p)
{
    std::cout<<"Inside Thread :  "" : p = "<<p<<std::endl;
    std::chrono::milliseconds dura( 1000 );
    std::this_thread::sleep_for( dura );
    *p = 19;
}

void startNewThread()
{
    int i = 10;
    std::cout<<"Inside Main Thread :  "" : i = "<<i<<std::endl;
    std::thread t(newThreadCallback,&i);
    t.detach();
    std::cout<<"Inside Main Thread :  "" : i = "<<i<<std::endl;
}

int main()
{
    startNewThread();
    std::chrono::milliseconds dura( 2000 );
    std::this_thread::sleep_for( dura );
    return 0;
}
```

当传递指向堆上分配的内存的指针时应该小心，有可能会导致不可预期的错误

Similarly be careful while passing pointer to memory located on heap to thread. Because it might be possible that some thread deletes that memory before new thread tries to access it.
In such scenario accessing invalid address can cause unexpected behaviour.
For example,

```c++
#include <iostream>
#include <thread>

void newThreadCallback(int * p)
{
    std::cout<<"Inside Thread :  "" : p = "<<p<<std::endl;
    std::chrono::milliseconds dura( 1000 );
    std::this_thread::sleep_for( dura );
    *p = 19;
}

void startNewThread()
{
    int * p = new int();
    *p = 10;
    std::cout<<"Inside Main Thread :  "" : *p = "<<*p<<std::endl;
    std::thread t(newThreadCallback,p);
    t.detach();
    delete p;
    p = NULL;
}

int main()
{
    startNewThread();
    std::chrono::milliseconds dura( 2000 );
    std::this_thread::sleep_for( dura );
    return 0;
}
```

**How to pass references to thread**:

As arguments are copied to new threads stack so, if you need to pass references in common way i.e.

Check this,

```c++
#include <iostream>
#include <thread>

void threadCallback(int const & x)
{
    int & y = const_cast<int &>(x);
    y++;
    std::cout<<"Inside Thread x = "<<x<<std::endl;
}

int main()
{
    int x = 9;
    std::cout<<"In Main Thread : Before Thread Start x = "<<x<<std::endl;
    std::thread threadObj(threadCallback, x);
    threadObj.join();
    std::cout<<"In Main Thread : After Thread Joins x = "<<x<<std::endl;
    return 0;
}
```

Its output is

*In Main Thread : Before Thread Start x = 9* 
*Inside Thread x = 10* 
*In Main Thread : After Thread Joins x = 9* 

即使线程回调函数接受应用作为参数，在线程函数内部会有所变化，但是在线程函数外部，是不可见的，这是因为线程函数中的x只是栈上临时值的一个拷贝

Even if threadCallback accepts arguments as reference but still changes done it are not visible outside the thread.
Its because x in the thread function threadCallback is reference to the temporary value copied at the new thread’s stack.

How to fix this ?

Using `std::ref()` i.e.

```c++
#include <iostream>
#include <thread>

void threadCallback(int const & x)
{
    int & y = const_cast<int &>(x);
    y++;
    std::cout<<"Inside Thread x = "<<x<<std::endl;
}

int main()
{
    int x = 9;
    std::cout<<"In Main Thread : Before Thread Start x = "<<x<<std::endl;
    std::thread threadObj(threadCallback,std::ref(x));
    threadObj.join();
    std::cout<<"In Main Thread : After Thread Joins x = "<<x<<std::endl;
    return 0;
}
```

Its output is

*In Main Thread : Before Thread Start x = 9* 

*Inside Thread x = 10* 

*In Main Thread : After Thread Joins x = 10* 

**Assigning pointer to member function of a class as thread function:**

Pass the pointer to member function as callback function and pass pointer to Object as second argument.

For example,

```c++
#include <iostream>
#include <thread>

class DummyClass {
public:
    DummyClass()
    {}
    DummyClass(const DummyClass & obj)
    {}
    void sampleMemberFunction(int x)
    {
        std::cout<<"Inside sampleMemberFunction "<<x<<std::endl;
    }
};

int main() 
{
    DummyClass dummyObj;
    int x = 10;
    std::thread threadObj(&DummyClass::sampleMemberFunction,&dummyObj, x);
    threadObj.join();
    return 0;
}
```