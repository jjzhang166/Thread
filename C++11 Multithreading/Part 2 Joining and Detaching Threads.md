

# C++11 Multithreading – Part 2: Joining and Detaching Threads

***join thread:***

Once a thread is started then another thread can wait for this new thread to finish. For this another need need to call join() function on the std::thread object i.e.


`std::thread th(funcPtr);` 
`th.join();`

Lets see an example ,
Suppose Main Thread has to start 10 Worker Threads and after starting all these threads, main function will wait for them to finish. After joining all the threads main function will continue,

```C++
#include <iostream>
#include <thread>
#include <algorithm>

class WorkerThread
{
  public:   
      void operator()()     
      {
          std::cout<<"Worker Thread "<<std::this_thread::get_id()<<" is Executing"<<std::endl;
      }
};

int main()  
{
    std::vector<std::thread> threadList;
    for(int i = 0; i < 10; i++)
    {
        threadList.push_back( std::thread( WorkerThread() ) );
    }
    // Now wait for all the worker thread to finish i.e.
    // Call join() function on each of the std::thread object
    std::cout<<"wait for all the worker thread to finish"<<std::endl;
    std::for_each(threadList.begin(),threadList.end(), std::mem_fn(&std::thread::join));
    std::cout<<"Exiting from Main Thread"<<std::endl;
    return 0;
 }
```

***Detaching Threads:***

Detached threads are also called daemon / Background threads.  To detach a thread we need to call std::detach() function on std::thread object i.e.

```c++
std::thread th(funcPtr)
th.detach()
```


After calling detach(), std::thread object is no longer associated with the actual thread of execution.

Be careful with calling detach() and join() on Thread Handles

***Case 1: Never call join() or detach() on std::thread object with no associated executing thread***


```C++
std::thread threadObj( (WorkerThread()) );
threadObj.join();
threadObj.join(); // It will cause Program to Terminate
```
When a join() function is called on an thread object, then when this join(0 returns then that std::thread object has no associated thread with it. In case again join() function is called on such object then it will cause the program to Terminate.

Similarly calling detach() makes the std::thread object not linked with any thread function. In that case calling detach(0 function twice on an std::thread object will cause the program to terminate.


```c++
std::thread threadObj( (WorkerThread()) );
threadObj.detach();
threadObj.detach(); // It will cause Program to Terminate
```
Therefore, before calling join() or detach() we should check if thread is join-able every time i.e.


```c++
std::thread threadObj( (WorkerThread()) );
if(threadObj.joinable())
{
    std::cout<<"Detaching Thread "<<std::endl;
    threadObj.detach();
}
if(threadObj.joinable())    
{
    std::cout<<"Detaching Thread "<<std::endl;
    threadObj.detach();
}

std::thread threadObj2( (WorkerThread()) );
if(threadObj2.joinable())
{
    std::cout<<"Joining Thread "<<std::endl;
    threadObj2.join();
}
if(threadObj2.joinable())    
{
    std::cout<<"Joining Thread "<<std::endl;
    threadObj2.join();
}
```
***Case 2 : Never forget to call either join or detach on a std::thread object with associated executing thread*** 

If  neither join or detach is called with a std::thread object that has associated executing thread then during that object’s destruct-or it will terminate the program.
Because inside the destruct-or it checks if Thread is Still Join-able then Terminate the program i.e.

```c++
#include <iostream>
#include <thread>
#include <algorithm>
class WorkerThread
{
public:
    void operator()()     
    {
        std::cout<<"Worker Thread "<<std::endl;
    }
};
int main()  
{
    std::thread threadObj( (WorkerThread()) );
    // Program will terminate as we have't called either join or detach with the std::thread object.
    // Hence std::thread's object destructor will terminate the program
    return 0;
}
```


Similarly we should not forget call either join() or detach() in case of exceptions. To prevents with we should use RESOURCE ACQUISITION IS INITIALIZATION (RAII) i.e.
```c++
#include <iostream>
#include <thread>
class ThreadRAII
{
    std::thread & m_thread;
    public:
        ThreadRAII(std::thread  & threadObj) : m_thread(threadObj)
        {
            
        }
        ~ThreadRAII()
        {
            // Check if thread is joinable then detach the thread
            if(m_thread.joinable())
            {
                m_thread.detach();
            }
        }
};
void thread_function()
{
    for(int i = 0; i < 10000; i++);
        std::cout<<"thread_function Executing"<<std::endl;
}
 
int main()  
{
    std::thread threadObj(thread_function);
    
    // If we comment this Line, then program will crash
    ThreadRAII wrapperObj(threadObj);
    return 0;
}
```